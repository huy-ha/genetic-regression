#include "Expression.hpp"
#include <iostream>
#include "Constant.hpp"
#include "Cos.hpp"
#include "Sin.hpp"
#include "Plus.hpp"
#include "Minus.hpp"
#include "Multiply.hpp"
#include "Divide.hpp"
#include "Variable.hpp"

namespace SymbolicRegression
{
Expression::Expression()
{
    m_func = 0;
    m_order = -1;
}

std::shared_ptr<Expression> Expression::GenerateRandomExpression()
{
    // prioritize constants
    if (RandomF() > 0.3f)
    {
        return RandomF() > 0.5f ? std::shared_ptr<Expression>(new SymbolicRegression::Constant()) : std::shared_ptr<Expression>(new SymbolicRegression::Variable());
    }
    // consider operators

    //trig functions with low probability
    if (RandomF() > 0.8f)
    {
        // equal probability of cos and sin
        return RandomF() > 0.5f ? std::shared_ptr<Expression>(new SymbolicRegression::Cos()) : std::shared_ptr<Expression>(new SymbolicRegression::Sin());
    }
    float p = RandomF();
    //equal probabilty of binary opertaors
    if (p > (3.0f / 4.0f))
    {
        return std::shared_ptr<Expression>(new SymbolicRegression::Plus());
    }
    else if (p > (2.0f / 4.0f))
    {
        return std::shared_ptr<Expression>(new SymbolicRegression::Minus());
    }
    else if (p > (1.0f / 4.0f))
    {
        return std::shared_ptr<Expression>(new SymbolicRegression::Multiply());
    }
    else
    {
        return std::shared_ptr<Expression>(new SymbolicRegression::Divide());
    }
}

std::function<float(float)> Expression::ToFunction()
{
    return m_func;
}

float Expression::Evaluate(float x)
{
    return m_func(x);
}

float Expression::operator()(float x)
{
    return Evaluate(x);
}

void Expression::AddSubexpression(std::shared_ptr<Expression> subexpression)
{
    m_subexpressions.push_back(subexpression);
}

float Expression::RandomF()
{
    return RandomF(0, 1);
}

float Expression::RandomF(float min, float max)
{
    return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (max - min) + min;
}

} // namespace SymbolicRegression