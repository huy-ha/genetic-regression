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
#include "../engine/OutputLogger.hpp"

namespace SymbolicRegression
{
using namespace std;
function<bool(
    const shared_ptr<Expression> &,
    const shared_ptr<Expression> &)>
    Expression::FitnessComparer = [](const shared_ptr<Expression> &a,
                                     const shared_ptr<Expression> &b) -> bool {
    return a->Fitness() > b->Fitness();
};

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
    function<float(float)> f = ToFunction();
    float AbsoluteErrorSum = 0;
    for_each(Config::Data->begin(), Config::Data->end(), [&](tuple<float, float> datapoint) {
        AbsoluteErrorSum += abs(f(get<0>(datapoint)) - get<1>(datapoint));
    });
    float AbsoluteMeanError = AbsoluteErrorSum / Config::Data->size();
    OutputLogger::Evaluations = OutputLogger::Evaluations + 1;
    return 100 / (AbsoluteMeanError + 1);
}

shared_ptr<Expression> Expression::GenerateRandomExpression(bool noConstant)
{
    // prioritize constants
    if (RandomF() > 0.3f)
    {
        if (RandomF() > 0.5f || noConstant)
        {
            return shared_ptr<Expression>(new SymbolicRegression::Variable());
        }
        else
        {
            return shared_ptr<Expression>(new SymbolicRegression::Constant());
        }
    }
    // consider operators

    //trig functions with low probability
    if (RandomF() > 0.8f)
    {
        // equal probability of cos and sin
        return RandomF() > 0.5f ? shared_ptr<Expression>(new SymbolicRegression::Cos()) : shared_ptr<Expression>(new SymbolicRegression::Sin());
    }
    float p = RandomF();
    //equal probabilty of binary opertaors
    if (p > (3.0f / 4.0f))
    {
        return shared_ptr<Expression>(new SymbolicRegression::Plus());
    }
    else if (p > (2.0f / 4.0f))
    {
        return shared_ptr<Expression>(new SymbolicRegression::Minus());
    }
    else if (p > (1.0f / 4.0f))
    {
        return shared_ptr<Expression>(new SymbolicRegression::Multiply());
    }
    else
    {
        return shared_ptr<Expression>(new SymbolicRegression::Divide());
    }
}

function<float(float)> Expression::ToFunction() const
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
        throw exception("Uncalculated fitness!");
    return m_fitness > e.m_fitness;
}

void Expression::AddSubexpression(shared_ptr<Expression> subexpression)
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