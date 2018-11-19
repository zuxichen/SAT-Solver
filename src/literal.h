
#ifndef SAT_LITERAL_H
#define SAT_LITERAL_H

#include <ostream>

namespace sat
{
    class Literal
    {
    public:
        explicit Literal(const int rep);
        int id() const;
        // false means negated
        bool polarity() const;
        Literal negate() const;
        bool evaluate(const bool value) const;
        int rep() const;
        bool operator==(const Literal& t) const;

    private:
        Literal(const int varId, const bool varPolarity);
        int var;
        bool pol;
    };

    std::ostream& operator<<(std::ostream& out, const sat::Literal& t);
}

namespace std
{
    template <>
    struct hash<sat::Literal>
    {
        size_t operator()(const sat::Literal& t) const
        {
            return hash<int>()(t.rep());
        }
    };
}

#endif // SAT_LITERAL_H
