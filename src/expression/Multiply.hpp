#ifndef _MULTIPLY_HPP_
#define _MULTIPLY_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Multiply : public Expression
{
public:
    inline Multiply() : Expression()
    {
        m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression()));
        m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression()));
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) * m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }
    inline virtual std::string ToString() const override
    {
        return m_subexpressions[0]->ToString() + " * " + m_subexpressions[1]->ToString();
    }
};
} // namespace SymbolicRegression
#endif