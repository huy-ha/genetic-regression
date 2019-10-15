#ifndef _DIVIDE_HPP_
#define _DIVIDE_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Divide : public Expression
{
public:
    inline Divide(int level) : Expression(level)
    {
        if (level + 1 == Config::GetInt("MaxDepth"))
        {
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
        }
        else
        {
            m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression(level + 1)));
            m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression(level + 1, false, true)));
        }
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) / m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }

    inline Divide(const Divide &other) : Expression(other)
    {
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) / m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }
    inline virtual std::string ToString() const override
    {
        return m_subexpressions[0]->ToString() + " / " + m_subexpressions[1]->ToString();
    }
};
} // namespace SymbolicRegression
#endif