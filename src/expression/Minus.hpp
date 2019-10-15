#ifndef _MINUS_HPP_
#define _MINUS_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Minus : public Expression
{
public:
    inline Minus() : Expression()
    {
        m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression()));
        m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression()));
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) - m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }
    inline Minus(const Minus &other) : Expression(other)
    {
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) - m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    };
    inline std::string ToString() const override
    {
        std::string tmp = m_subexpressions[1]->ToString();
        if (m_subexpressions[1]->ToString().at(0) == '-')
            return "(" + m_subexpressions[0]->ToString() + " + " + tmp.substr(1, tmp.length()) + ")";
        return "(" + m_subexpressions[0]->ToString() + " - " + tmp + ")";
    }
};
} // namespace SymbolicRegression
#endif