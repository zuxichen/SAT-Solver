
#ifndef SAT_SOLVER_H
#define SAT_SOLVER_H

#include "clause.h"
#include "assignment.h"
#include "formula.h"
#include <vector>
#include <stack>
#include <deque>
#include <set>
#include <unordered_set>
#include <functional>

namespace sat
{
    enum class Result { Satisfiable, Unsatisfiable, Unknown };
    std::ostream& operator<<(std::ostream& out, const Result& v);

    class BaseSolver
    {
    public:
        virtual void solve(const int timeLimitSeconds, const bool verbose) = 0;
        virtual void cancel() = 0;
    };

    class Solver : public BaseSolver
    {
    public:
        explicit Solver(Formula&& formula);
        virtual void solve(const int timeLimitSeconds, const bool verbose);
        virtual void cancel();

    private:
        void unitPropagation();
        void pureLiteralElimination();
        void processSingleLiteralClauses();
        void makeDecision();
        void backtrack(int level);
        void assertLiteral(const Literal& t);
        void updateWatches(const Literal& t);
        void addToWatchList(const Literal& t, Clause& clause);
        int analyzeConflict();
        void markConflict(const Clause& clause);
        void clearConflict();
        void addLiterals(const Clause& clause);
        void setReason(const Literal& t, const Clause& clause);
        void printResult(const bool verbose);

        Formula formula;
        Result result = Result::Unknown;
        bool canceled = false;

        Assignment assignment;

        template <class T> using LiteralMap = std::unordered_map<Literal, T>;
        using ClauseRef = std::reference_wrapper<Clause>;
        using ConstClauseRef = std::reference_wrapper<const Clause>;
        using ClauseList = std::vector<ClauseRef>;

        LiteralMap<ClauseList> watchList;
        LiteralMap<ConstClauseRef> upReasons;
        std::deque<Literal> propagationQueue;

        struct LiteralComparator {
            const Assignment& assignment;
            explicit LiteralComparator(const Assignment& assignment): assignment(assignment) {}
            bool operator()(const Literal& t1, const Literal& t2) const {
                return assignment.isBefore(t1, t2);
            }
        };
        LiteralComparator comparator;
        // contains only one ploarity of any literal
        std::set<Literal, LiteralComparator> confLitCurrLvl;
        std::set<Literal, LiteralComparator> confLitLowerLvl;
        std::unordered_set<int> conflictLiterals;
        bool hasConflict = false;
        const Clause* conflictClause;
        Clause lastLearnedClause = Clause(std::vector<int>());
    };
}

#endif // SAT_SOLVER_H
