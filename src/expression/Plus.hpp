#ifndef _PLUS_HPP_
#define _PLUS_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class Plus : public Expression
{
public:
    inline Plus() : Expression()
    {
        m_subexpressions.push_back(shared_ptr<Expression>(Expression::GenerateRandomExpression()));
        m_subexpressions.push_back(shared_ptr<Expression>(Expression::GenerateRandomExpression()));
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) + m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }
    inline Plus(const Plus &other) : Expression(other)
    {
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) + m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    };
    inline virtual string ToString() const override
    {
        string tmp = m_subexpressions[1]->ToString();
        if (m_subexpressions[1]->ToString().at(0) == '-')
            return "(" + m_subexpressions[0]->ToString() + " - " + tmp.substr(1, tmp.length()) + ")";
        return "(" + m_subexpressions[0]->ToString() + " + " + m_subexpressions[1]->ToString() + ")";
    }
};
} // namespace SymbolicRegression
#endif