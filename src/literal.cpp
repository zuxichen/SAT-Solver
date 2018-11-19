
#include "literal.h"

sat::Literal::Literal(const int rep)
{
    var = rep > 0 ? rep : -rep;
    pol = rep > 0;
}

sat::Literal::Literal(const int varId, const bool varPolarity):var(varId), pol(varPolarity) {}

int sat::Literal::id() const
{
    return var;
}

bool sat::Literal::polarity() const
{
    return pol;
}

sat::Literal sat::Literal::negate() const
{
    return Literal(var, !pol);
}

bool sat::Literal::evaluate(const bool value) const
{
    return pol == value;
}

int sat::Literal::rep() const
{
    return pol ? var : -var;
}

bool sat::Literal::operator==(const Literal& t) const
{
    return var == t.var && pol == t.pol;
}

std::ostream& sat::operator<<(std::ostream& out, const sat::Literal& t)
{
    return out << t.rep();
}
