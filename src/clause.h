
#ifndef SAT_CLAUSE_H
#define SAT_CLAUSE_H

#include "literal.h"
#include <ostream>
#include <vector>

namespace sat
{
    class Assignment;
    class Clause
    {
    public:
        explicit Clause(const std::vector<int>& input);
        explicit Clause(std::vector<Literal>& input);
        std::size_t size() const;
        bool empty() const;

        const Literal& w1() const;
        const Literal& w2() const;
        void swapWatches();
        bool moveW2(Assignment& assignment);

        class const_iterator
        {
        public:
            using value_type = Literal;
            using reference = const value_type&;
            using pointer = const value_type*;
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;

            const_iterator() {}
            const_iterator operator++() { ++p; return *this; }
            const_iterator operator++(int) { const_iterator i = *this; ++p; return i; }
            reference operator*() { return *p; }
            pointer operator->() { return &(*p); }
            bool operator==(const const_iterator& other) const { return p == other.p; }
            bool operator!=(const const_iterator& other) const { return p != other.p; }
        private:
            const_iterator(const std::vector<Literal>::const_iterator it): p(it) {}
            std::vector<Literal>::const_iterator p;
            friend class Clause;
        };

        const_iterator begin() const;
        const_iterator end() const;

    private:
        std::vector<Literal> literals;
    };

    std::ostream& operator<<(std::ostream& out, const sat::Clause& clause);
}

#endif // SAT_CLAUSE_H
