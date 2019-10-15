#ifndef _COS_HPP_
#define _COS_HPP_
#include "Expression.hpp"
#include <math.h>
namespace SymbolicRegression
{
class Cos : public Expression
{
public:
    inline Cos(shared_ptr<Expression> parent) : Expression(parent)
    {
        m_subexpressions.push_back(Expression::GenerateRandomExpression(m_this, true, true));
        m_func = [&](float x) {
            return cosf(m_subexpressions[0]->ToFunction()(x));
        };
        m_order = 1;
    }

    inline Cos(const Cos &other) : Expression(other)
    {
        m_func = [&](float x) {
            return cosf(m_subexpressions[0]->ToFunction()(x));
        };
        m_order = 1;
    }

    inline virtual std::string ToString() const override
    {
        return "cos(" + m_subexpressions[0]->ToString() + ")";
    }
};
} // namespace SymbolicRegression
#endif