#ifndef _PLUS_HPP_
#define _PLUS_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class Plus : public Expression
{
public:
    inline Plus(int level) : Expression(level)
    {
        if (level + 1 == Config::GetInt("MaxDepth"))
        {
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
        }
        else
        {
            m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression(level + 1)));
            m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression(level + 1)));
        }
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) + m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }

    inline Plus(int level, shared_ptr<Expression> e1, shared_ptr<Expression> e2) : Expression(level)
    {
        m_subexpressions.push_back(e1);
        m_subexpressions.push_back(e2);

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