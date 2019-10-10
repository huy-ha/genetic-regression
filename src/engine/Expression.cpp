#include "Expression.hpp"
#include <iostream>

Expression::Expression()
{
    m_func = 0;
    m_expressionFunc = 0;
    m_order = -1;
}

Expression::Expression(std::function<float(float)> f)
{
    m_func = f;
    m_order = NumArgs(f);
}

std::function<float(float)> Expression::ToFunction()
{
    if (m_subexpressions.size() == m_order)
    {
        if (m_order == 0)
            return m_func;
        auto func = [&](float x) {
            return m_func(m_subexpressions[0]->ToFunction()(x));
        };
        return func;
    }
    std::cerr << "ERROR: Wrong subexpressions count!" << std::endl;
    throw new std::exception("Wrong subexpressions count!");
}

float Expression::Evaluate(float x)
{
    if (m_expressionFunc == 0)
        m_expressionFunc = ToFunction();
    return m_expressionFunc(x);
}

float Expression::operator()(float x)
{
    return Evaluate(x);
}

void Expression::AddSubexpression(Expression *subexpression)
{
    m_subexpressions.push_back(subexpression);
}