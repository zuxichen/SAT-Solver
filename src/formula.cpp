
#include "formula.h"

sat::Formula::Formula(const int numVars): numVars(numVars)
{}

int sat::Formula::numVariables() const
{
    return numVars;
}

int sat::Formula::numClauses() const
{
    return clauses.size();
}

sat::Clause& sat::Formula::addClause(Clause&& clause)
{
    clauses.push_back(std::make_unique<Clause>(std::move(clause)));
    return *clauses.back();
}

sat::Clause& sat::Formula::addClause(const Clause& clause)
{
    clauses.push_back(std::make_unique<Clause>(clause));
    return *clauses.back();
}

sat::Formula::iterator sat::Formula::begin()
{
    return iterator(clauses.begin());
}

sat::Formula::iterator sat::Formula::end()
{
    return iterator(clauses.end());
}

sat::Formula::const_iterator sat::Formula::begin() const
{
    return const_iterator(const_cast<std::vector<std::unique_ptr<Clause>>&>(clauses).begin());
}

sat::Formula::const_iterator sat::Formula::end() const
{
    return const_iterator(const_cast<std::vector<std::unique_ptr<Clause>>&>(clauses).end());
}
