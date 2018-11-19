
#ifndef SAT_ASSIGNMENT_H
#define SAT_ASSIGNMENT_H

#include "literal.h"
#include <vector>
#include <unordered_map>

namespace sat
{
    // tracks assignment of variables and decisions made
    class Assignment
    {
    public:
        Assignment(const int numVars);
        int unassigned() const;
        int total() const;
        bool allAssigned() const;

        bool isAssigned(const Literal& t) const;
        bool isTrue(const Literal& t) const;
        bool isFalse(const Literal& t) const;
        bool evaluate(const Literal& t) const;  // can throw

        // if t1 is assigned before t2
        bool isBefore(const Literal& t1, const Literal& t2) const;

        int currentLevel() const;
        int level(const Literal& t) const; // can throw
        Literal lastDecision() const;
        Literal lastAssigned() const;
        std::vector<Literal> decisions() const;
        bool isDecided(const Literal& t) const;

        void decide(const Literal& t);
        void assign(const Literal& t);

        void backtrackTo(const int level);
        void backtrack(const Literal& t);

        std::vector<bool> values() const;

    private:
        struct Value
        {
            int var;
            bool value;
            int level;
        };
        void checkIsAssigned(const Literal& t) const;
        const Value& getValue(const int id) const;
        Literal toLiteral(const Value& v) const;
        void removeVar(const int varId);

        const int numVars;
        mutable std::unordered_map<int, std::size_t> varMap;
        std::vector<Value> varStack;
        std::vector<int> varDecisions;
    };
}

#endif // SAT_ASSIGNMENT_H
