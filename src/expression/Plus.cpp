#include "Plus.hpp"

namespace SymbolicRegression
{
Plus::Plus() : Expression()
{
    m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression()));
    m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression()));
    m_func = [&](float x) {
        return m_subexpressions[0]->ToFunction()(x) + m_subexpressions[1]->ToFunction()(x);
    };
    m_order = 2;
}


std::string Plus::ToString() const
{
    std::string tmp = m_subexpressions[1]->ToString();
    if (m_subexpressions[1]->ToString().at(0) == '-')
        return "(" + m_subexpressions[0]->ToString() + " - " + tmp.substr(1, tmp.length()) + ")";
    return "(" + m_subexpressions[0]->ToString() + " + " + m_subexpressions[1]->ToString() + ")";
}
} // namespace SymbolicRegression