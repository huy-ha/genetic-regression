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
#include "../engine/Config.hpp"
#include <functional>
#include <algorithm>

namespace SymbolicRegression
{
Expression::Expression()
{
    m_func = 0;
    m_order = -1;
}

float Expression::Fitness()
{
    if (m_fitness == -1)
    {
        m_fitness = CalculateFitness();
    }
    return m_fitness;
}

float Expression::CalculateFitness() const
{
    using namespace std;
    std::function<float(float)> f = ToFunction();
    float AbsoluteErrorSum = 0;
    std::for_each(Config::Data->begin(), Config::Data->end(), [&](std::tuple<float, float> datapoint) {
        AbsoluteErrorSum += f(std::get<0>(datapoint)) - std::get<1>(datapoint);
    });
    float AbsoluteMeanError = AbsoluteErrorSum / Config::Data->size();

    return 100 / (AbsoluteMeanError + 1);
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

std::function<float(float)> Expression::ToFunction() const
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

bool Expression::operator<(const Expression &e)
{
    if (m_fitness == -1 || e.m_fitness == -1)
        throw std::exception("Uncalculated fitness!");
    return m_fitness > e.m_fitness;
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