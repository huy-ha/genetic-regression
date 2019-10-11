#ifndef _SIN_HPP_
#define _SIN_HPP_
#include "Expression.hpp"
#include <math.h>
namespace SymbolicRegression
{
class Sin : public Expression
{
public:
    inline Sin() : Expression(sinf)
    {
        m_subexpressions.push_back(new SymbolicRegression::Constant());
    }
    inline virtual std::string ToString() const override
    {
        return "sin(" + m_subexpressions[0]->ToString() + ")";
    }
};
} // namespace SymbolicRegression
#endif