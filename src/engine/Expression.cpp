#include "Expression.hpp"
#include <iostream>

std::function<float(float)> Expression::ToFunction()
{
    return [&](float x) { return m_func(x); };
}

float Expression::Evaluate(float x)
{
    if (m_expressionFunc == 0)
        m_expressionFunc = ToFunction();
    return m_expressionFunc(x);
}
