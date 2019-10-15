#ifndef _MULTIPLY_HPP_
#define _MULTIPLY_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class Multiply : public Expression
{
public:
    inline Multiply(shared_ptr<Expression> parent) : Expression(parent)
    {
        m_subexpressions.push_back(shared_ptr<Expression>(Expression::GenerateRandomExpression(m_this)));
        m_subexpressions.push_back(shared_ptr<Expression>(Expression::GenerateRandomExpression(m_this)));
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) * m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }

    inline Multiply(const Multiply &other) : Expression(other)
    {
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) * m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }

    inline virtual string ToString() const override
    {
        return m_subexpressions[0]->ToString() + " * " + m_subexpressions[1]->ToString();
    }
};
} // namespace SymbolicRegression
#endif