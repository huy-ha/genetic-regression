#ifndef _COS_HPP_
#define _COS_HPP_
#include "Expression.hpp"
#include <math.h>
namespace SymbolicRegression
{
class Cos : public Expression
{
public:
    inline Cos() : Expression()
    {
        m_subexpressions.push_back(Expression::GenerateRandomExpression());
    }
    inline virtual std::string ToString() const override
    {
        return "cos(" + m_subexpressions[0]->ToString() + ")";
    }
};
} // namespace SymbolicRegression
#endif