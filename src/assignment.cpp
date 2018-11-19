
#include "assignment.h"
#include <string>

using namespace std::string_literals;

sat::Assignment::Assignment(const int numVars): numVars(numVars)
{
    if (numVars <= 0) {
        throw std::out_of_range("invalid numVars "s + std::to_string(numVars));
    }
}

int sat::Assignment::total() const
{
    return numVars;
}

int sat::Assignment::unassigned() const
{
    return numVars - varStack.size();
}

bool sat::Assignment::allAssigned() const
{
    return unassigned() == 0;
}

bool sat::Assignment::isAssigned(const Literal& t) const
{
    if (t.id() <= 0 || t.id() > numVars) {
        throw std::out_of_range("invalid var "s + std::to_string(t.id()));
    }
    if (varMap.find(t.id()) != varMap.end()) {
        if (varMap.at(t.id()) < varStack.size()
                && getValue(t.id()).var == t.id()) {
            return true;
        } else {
            varMap.erase(t.id());
        }
    }
    return false;
}

void sat::Assignment::checkIsAssigned(const Literal& t) const
{
    if (!isAssigned(t)) {
        throw std::invalid_argument("var "s + std::to_string(t.id()) + " is not assigned"s);
    }
}

bool sat::Assignment::isTrue(const Literal& t) const
{
    return isAssigned(t) && evaluate(t);
}

bool sat::Assignment::isFalse(const Literal& t) const
{
    return isAssigned(t) && !evaluate(t);
}

bool sat::Assignment::evaluate(const Literal& t) const
{
    checkIsAssigned(t);
    return t.evaluate(getValue(t.id()).value);
}

const sat::Assignment::Value& sat::Assignment::getValue(const int id) const
{
    return varStack.at(varMap.at(id));
}

bool sat::Assignment::isBefore(const sat::Literal& t1, const sat::Literal& t2) const
{
    checkIsAssigned(t1);
    checkIsAssigned(t2);
    return t1.id() != t2.id() && varMap.at(t1.id()) < varMap.at(t2.id());
}

int sat::Assignment::currentLevel() const
{
    return varDecisions.size();
}

int sat::Assignment::level(const Literal& t) const
{
    checkIsAssigned(t);
    return getValue(t.id()).level;
}

sat::Literal sat::Assignment::toLiteral(const sat::Assignment::Value& v) const
{
    return Literal(v.value ? v.var : -v.var);
}

sat::Literal sat::Assignment::lastDecision() const
{
    if (currentLevel() == 0) {
        throw std::out_of_range("No decision made");
    }
    return toLiteral(getValue(varDecisions.back()));
}

sat::Literal sat::Assignment::lastAssigned() const
{
    if (varStack.empty()) {
        throw std::out_of_range("No assignment made");
    }
    return toLiteral(varStack.back());
}

std::vector<sat::Literal> sat::Assignment::decisions() const
{
    std::vector<Literal> ret;
    for (int i: varDecisions) {
        ret.push_back(toLiteral(getValue(i)));
    }
    return ret;
}

bool sat::Assignment::isDecided(const Literal& t) const
{
    return isAssigned(t) && varDecisions.at(level(t)-1) == t.id();
}

void sat::Assignment::decide(const Literal& t)
{
    // making a decsion, increment decision level
    varDecisions.push_back(t.id());
    assign(t);
}

void sat::Assignment::assign(const Literal& t)
{
    if (t.id() <= 0 || t.id() > numVars) {
        throw std::out_of_range("invalid var "s + std::to_string(t.id()));
    }
    if (!isAssigned(t)) {
        varStack.push_back({t.id(), t.polarity(), currentLevel()});
        varMap[t.id()] = varStack.size() - 1;
    } else {
        if (varMap.at(t.id()) == varStack.size()-1) {
            varStack[varStack.size()-1].value = t.polarity();
        } else {
            throw std::invalid_argument("reassigning var "s + std::to_string(t.id())
                                        + " that is not the last assigned");
        }
    }
}

void sat::Assignment::backtrackTo(const int level)
{
    if (level < 0 || level > currentLevel()) {
        throw std::out_of_range("invalid target level "s + std::to_string(level));
    }
    // remove all decisions made after level
    // preserving decisions up to and including level
    // remove all variable assignments made >= level+1
    // index = level-1
    removeVar(varDecisions.at(level));
    varDecisions.resize(level);
}

void sat::Assignment::backtrack(const Literal& t)
{
    checkIsAssigned(t);
    const int lvl = level(t);
    removeVar(t.id());
    // level = vector size
    varDecisions.resize(lvl);
}

void sat::Assignment::removeVar(const int varId)
{
    varStack.resize(varMap.at(varId));
    varMap.erase(varId);
}

std::vector<bool> sat::Assignment::values() const
{
    std::vector<bool> ret;
    for (int i = 1; i <= numVars; i++) {
        const bool val = varStack.at(varMap.at(i)).value;
        ret.push_back(val);
    }
    return ret;
}
