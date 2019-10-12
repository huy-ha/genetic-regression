#ifndef _SIN_HPP_
#define _SIN_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Sin : public Expression
{
public:
    Sin();
    inline virtual std::string ToString() const override
    {
        return "sin(" + m_subexpressions[0]->ToString() + ")";
    }
};
} // namespace SymbolicRegression
#endif