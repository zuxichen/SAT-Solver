
#include "solver.h"
#include "timer.h"
#include "debug.h"
#include <ostream>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <iostream>

std::ostream& sat::operator<<(std::ostream& out, const sat::Result& v)
{
    static std::map<Result, std::string> str;
    if (str.size() == 0) {
        str[Result::Satisfiable] = "sat";
        str[Result::Unsatisfiable] = "unsat";
        str[Result::Unknown] = "unknown";
    }
    return out << str.at(v);
}

void sat::Solver::printResult(const bool verbose)
{
    std::ostream& out = std::cout;
    out << result << std::endl;
    if (!verbose) {
        return;
    }
    if (result == Result::Unknown) {
        out << "Current decisions: ";
        for (auto& t: assignment.decisions()) {
            out << t << " ";
        }
    } else if (result == Result::Unsatisfiable) {
        out << "Last learned clause: " << lastLearnedClause;
    } else {
        std::vector<bool> values = assignment.values();
        for (int i = 0; i < (int) values.size(); i++) {
            out << (values.at(i) ? (i+1) : -(i+1)) << " ";
        }
        out << "0";
    }
    out << std::endl;
}

sat::Solver::Solver(Formula&& formula):
        formula(std::move(formula)),
        assignment(this->formula.numVariables()),
        comparator(assignment),
        confLitCurrLvl(comparator),
        confLitLowerLvl(comparator)
{}

void sat::Solver::solve(const int timeLimitSeconds, const bool verbose)
{
    Timer timer(timeLimitSeconds);
    timer.start();
    pureLiteralElimination();
    processSingleLiteralClauses();
    if (hasConflict) {
        result = Result::Unsatisfiable;
    }
    timer.stop();
    while (!canceled && result == Result::Unknown
                && (timeLimitSeconds <= 0 || !timer.timeout())) {
        timer.start();
        unitPropagation();
        if (hasConflict) {
            if (assignment.currentLevel() > 0) {
                int backtrackLevel = analyzeConflict();
                backtrack(backtrackLevel);
            } else {
                result = Result::Unsatisfiable;
            }
        } else {
            if (assignment.allAssigned()) {
                result = Result::Satisfiable;
            } else {
                makeDecision();
            }
        }
        timer.stop();
    }
    printResult(verbose);
}

void sat::Solver::cancel()
{
    canceled = true;
}

// eliminates literals that never appeared or only appeared in one polarity
void sat::Solver::pureLiteralElimination()
{
    std::unordered_map<int, signed char> literals;
    for (auto& clause: formula) {
        for (const Literal& t: clause) {
            signed char p = t.polarity() ? 1 : -1;
            if (literals.find(t.id()) != literals.end()) {
                if (literals.at(t.id()) != p) {
                    literals[t.id()] = 0;
                }
            } else {
                literals[t.id()] = p;
            }
        }
    }

    for (int v = 1; v <= formula.numVariables(); v++) {
        if (literals.find(v) == literals.end()) {
            // never appeared in any clause, value doesn't matter
            assignment.assign(Literal(v));
        } else {
            if (literals.at(v) != 0) {
                // pure literal, only one polarity
                assignment.assign(Literal(v * (literals.at(v) > 0 ? 1 : -1)));
            }
        }
    }
}

void sat::Solver::processSingleLiteralClauses()
{
    std::vector<Literal> literals;
    for (Clause& clause: formula) {
        literals.clear();
        bool isAlreadyTrue = false;
        for (const Literal& t: clause) {
            if (assignment.isTrue(t)) {
                isAlreadyTrue = true;
                break;
            } else if (!assignment.isAssigned(t)){
                literals.push_back(t);
            }
        }
        if (isAlreadyTrue) {
            // clause is already true
            continue;
        }
        if (literals.empty()) {
            // all literals are false
            markConflict(clause);
            break;
        }
        if (literals.size() != clause.size()) {
            clause = Clause(literals);
        }
        if (clause.size() == 1) {
            // single literal clause, literal has to be true
            assertLiteral(*clause.begin());
            if (hasConflict) {
                break;
            }
        } else {
            // has 2 or more unassigned literals
            addToWatchList(clause.w1(), clause);
            addToWatchList(clause.w2(), clause);
        }
    }
}

void sat::Solver::addToWatchList(const Literal& t, Clause& clause)
{
    if (watchList.find(t) == watchList.end()) {
        watchList.insert({t, ClauseList()});
    }
    watchList.at(t).push_back(clause);
}

void sat::Solver::assertLiteral(const Literal& t)
{
    DBGPRINT("asserting " << t);
    assignment.assign(t);
    updateWatches(t.negate());
}

void sat::Solver::updateWatches(const Literal& t)
{
    if (watchList.find(t) != watchList.end()) {
        ClauseList& watchedClauses = watchList.at(t);
        ClauseList newWatchedClauses;
        DBGPRINT("update watches for " << t);
        for (Clause& clause: watchedClauses) {
            if (clause.w1() == t) {
                clause.swapWatches();
            }
            if (assignment.isTrue(clause.w1())) {
                newWatchedClauses.push_back(clause);
            } else if (clause.moveW2(assignment)) {
                // w2 is moved, t is not watched
                addToWatchList(clause.w2(), clause);
            } else if (assignment.isFalse(clause.w1())) {
                // w2 cannot move, w1 is false, clause is false
                markConflict(clause);
                DBGPRINT("has conflict propagating " << t << " at level "
                        << assignment.currentLevel() << ": " << clause);
                newWatchedClauses.push_back(clause);
            } else {
                if (!assignment.isAssigned(clause.w1())) {
                    Literal p = clause.w1();
                    propagationQueue.push_back(p);
                    DBGPRINT("reason for propagating " << p << ": " << clause);
                    // the literal might not get propagated, but it's ok
                    // in such case we'll never to get to analyze it, so it
                    // doesn't matter if there's an abandoned reason for it
                    setReason(p, clause);
                }
                newWatchedClauses.push_back(clause);
            }
        }
        watchList[t] = newWatchedClauses;
    }
}

void sat::Solver::markConflict(const sat::Clause& clause)
{
    hasConflict = true;
    conflictClause = &clause;
}

void sat::Solver::clearConflict()
{
    hasConflict = false;
    conflictClause = nullptr;
}

void sat::Solver::addLiterals(const sat::Clause& clause)
{
    DBGPRINT("adding literals from " << clause);
    for (const Literal& t: clause) {
        if (assignment.isAssigned(t)
            && conflictLiterals.find(t.id()) == conflictLiterals.end()) {
            // only process t once
            conflictLiterals.insert(t.id());
            if (assignment.level(t) == assignment.currentLevel()) {
                confLitCurrLvl.insert(t);
            } else if (assignment.level(t) > 0) {
                confLitLowerLvl.insert(t);
            }
        }
    }
}

void sat::Solver::setReason(const sat::Literal& t, const sat::Clause& clause)
{
    // stores const reference to clause as the reason
    auto pair = upReasons.insert({t, clause});
    if (!pair.second) {
        // update the value reference
        pair.first->second = clause;
    }
}

int sat::Solver::analyzeConflict()
{
    DBGPRINT("current level " << assignment.currentLevel());
    DBGPRINT("conflict clause " << *conflictClause);
    addLiterals(*conflictClause);
    DBGPRINTC("conflicts current level: ", confLitCurrLvl);
    DBGPRINTC("conflicts lower level: ", confLitLowerLvl);
    // not first UIP
    while (confLitCurrLvl.size() > 1) {
        // set is ordered by assignmend order, therefore the last one
        // is always the last assigned literal
        Literal t = *confLitCurrLvl.rbegin();
        // t in conflict clause must be false, thus the
        // opposite polarity was decided or propagated
        Literal tn = t.negate();
        DBGPRINT("analyzing assigned literal: " << tn);
        if (upReasons.find(tn) == upReasons.end()) {
            // if there is no reason, it means the literal is either
            // a decision or
            // a singal literal clause learned from an earlier backtrack
            continue;
        }
        const Clause& clause = upReasons.at(tn);
        addLiterals(clause);
        confLitCurrLvl.erase(t);
        DBGPRINTC("conflicts current level: ", confLitCurrLvl);
        DBGPRINTC("conflicts lower level: ", confLitLowerLvl);
    }
    std::vector<Literal> literals(confLitLowerLvl.begin(), confLitLowerLvl.end());
    // every conflict literals at lower level are false, so this one has to
    // be true to make the conflict/learned clause true; it effectively
    // becomes a unit clause
    // current assignment is false: -lastLiteral
    Literal lastLiteral = *confLitCurrLvl.begin();
    literals.push_back(lastLiteral);
    // if clause will have more than 1 literal
    if (literals.size() > 1) {
        // w1 will be last assigned literal, and w2 will be the second last
        std::reverse(literals.begin(), literals.end());
        // add to formula
        Clause& learnedClause = formula.addClause(Clause(literals));
        DBGPRINT("learned clause " << learnedClause);
        setReason(lastLiteral, learnedClause);
        addToWatchList(learnedClause.w1(), learnedClause);
        addToWatchList(learnedClause.w2(), learnedClause);
        lastLearnedClause = learnedClause;
    } else {
        DBGPRINT("learned single literal clause " << lastLiteral);
    }
    // unit propagation on the literal to make it true
    propagationQueue.push_back(lastLiteral);
    // backtrack to second highest level
    int backtrackLevel = 0;
    if (!confLitLowerLvl.empty()) {
        backtrackLevel = assignment.level(*confLitLowerLvl.rbegin());
    }
    confLitCurrLvl.clear();
    confLitLowerLvl.clear();
    conflictLiterals.clear();
    return backtrackLevel;
}

void sat::Solver::backtrack(int level)
{
    DBGPRINT("backtracking to level " << level);
    assignment.backtrackTo(level);
    clearConflict();
}

void sat::Solver::makeDecision()
{
    int v;
    for (v = 1; v <= formula.numVariables(); v++) {
        if (!assignment.isAssigned(Literal(v))) {
            break;
        }
    }
    Literal t(v);
    DBGPRINT("decision on " << v);
    assignment.decide(t);
    updateWatches(t.negate());
}

void sat::Solver::unitPropagation()
{
    while (!propagationQueue.empty()) {
        Literal t = propagationQueue.front();
        DBGPRINT("propagating " << t);
        if (!assignment.isAssigned(t)) {
            assertLiteral(t);
        }
        if (hasConflict) {
            break;
        }
        propagationQueue.pop_front();
    }
    // conflict
    propagationQueue.clear();
}
