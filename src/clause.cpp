
#include "clause.h"
#include "assignment.h"
#include <algorithm>

sat::Clause::Clause(const std::vector<int>& input)
{
    for (int p: input) {
        literals.push_back(Literal(p));
    }
}

sat::Clause::Clause(std::vector<Literal>& input):
    literals(std::move(input))
{}

std::size_t sat::Clause::size() const
{
    return literals.size();
}

bool sat::Clause::empty() const
{
    return literals.empty();
}

const sat::Literal& sat::Clause::w1() const
{
    return literals.at(0);
}

const sat::Literal& sat::Clause::w2() const
{
    return literals.at(1);
}

void sat::Clause::swapWatches()
{
    std::swap(*literals.begin(), *(literals.begin()+1));
}

bool sat::Clause::moveW2(Assignment& assignment)
{
    for (std::size_t i = 2; i < literals.size(); i++) {
        if (!assignment.isFalse(literals.at(i))) {
            std::swap(literals.at(1), literals.at(i));
            return true;
        }
    }
    return false;
}

sat::Clause::const_iterator sat::Clause::begin() const
{
    return const_iterator(literals.begin());
}

sat::Clause::const_iterator sat::Clause::end() const
{
    return const_iterator(literals.end());
}

std::ostream& sat::operator<<(std::ostream& out, const sat::Clause& clause)
{
    out << "[";
    if (!clause.empty()) {
        auto it = clause.begin();
        out << *it;
        ++it;
        for (; it != clause.end(); ++it) {
            out << " " << *it;
        }
    }
    return out << "]";
}
