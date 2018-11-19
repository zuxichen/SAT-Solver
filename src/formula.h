
#ifndef SAT_FORMULA_H
#define SAT_FORMULA_H

#include "clause.h"
#include <vector>
#include <memory>

namespace sat
{
    template <bool is_const = false>
    class Formula_iterator;

    // managing clauses
    class Formula
    {
    public:
        explicit Formula(const int numVars);
        int numVariables() const;
        int numClauses() const;

        Clause& addClause(Clause&& clause);
        Clause& addClause(const Clause& clause);

        using iterator = Formula_iterator<>;
        using const_iterator = Formula_iterator<true>;

        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;

    private:
        int numVars;
        std::vector<std::unique_ptr<Clause>> clauses;
    };

    template <bool is_const, class Const, class NonConst>
    struct choose;

    template <class Const, class NonConst>
    struct choose<true, Const, NonConst> {
        using type = Const;
    };

    template <class Const, class NonConst>
    struct choose<false, Const, NonConst> {
        using type = NonConst;
    };

    template <bool is_const>
    class Formula_iterator {
    public:
        using self_type = Formula_iterator;
        using value_type = Clause;
        using reference = typename choose<is_const, const value_type&, value_type&>::type;
        using pointer = typename choose<is_const, const value_type*, value_type*>::type;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;

        Formula_iterator() {}
        self_type operator++() { ++p; return *this; }
        self_type operator++(int) { self_type i = *this; ++p; return i; }
        reference operator*() const { return *(*p); }
        pointer operator->() const { return &(*(*p)); }
        bool operator==(const self_type& other) const { return p == other.p; }
        bool operator!=(const self_type& other) const { return p != other.p; }

    private:
        Formula_iterator(const std::vector<std::unique_ptr<Clause>>::iterator it): p(it) {}
        std::vector<std::unique_ptr<Clause>>::iterator p;
        friend class Formula;
    };
}

#endif // SAT_FORMULA_H
