#ifndef _COS_HPP_
#define _COS_HPP_
#include "Expression.hpp"
#include <math.h>
#include "../engine/Config.hpp"
namespace SymbolicRegression
{
class Cos : public Expression
{
public:
    inline Cos(int level) : Expression(level)
    {
        if (level + 1 == Config::GetInt("MaxDepth"))
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
        else
            m_subexpressions.push_back(Expression::GenerateRandomExpression(level + 1, true, false, true));
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