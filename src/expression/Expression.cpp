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
#include <typeinfo>

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

mutex Expression::randMutex;

Expression::Expression()
{
    m_func = 0;
    m_order = -1;
}

Expression::Expression(const Expression &other)
{
    m_order = other.m_order;
    m_func = 0;
    for (int i = 0; i < other.m_subexpressions.size(); i++)
    {
        m_subexpressions.push_back(Copy(other.m_subexpressions[i]));
    }
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
    OutputLogger::IncrementEvaluations();
    return 100 / (AbsoluteMeanError + 1);
}

shared_ptr<vector<shared_ptr<Expression>>> Expression::Collapse(shared_ptr<Expression> self)
{
    shared_ptr<vector<shared_ptr<Expression>>> output(new vector<shared_ptr<Expression>>());
    output->push_back(self);
    for (int i = 0; i < m_subexpressions.size(); i++)
    {
        auto collapsedSubExp = m_subexpressions[i]->Collapse(m_subexpressions[i]);
        output->insert(output->end(), collapsedSubExp->begin(), collapsedSubExp->end());
    }
    return output;
}

shared_ptr<Expression> Expression::GenerateRandomExpression(bool noConstant, bool noZero)
{
    // prioritize constants
    if (RandomF() > 0.3f)
    {
        if ((RandomF() > 0.5f || noConstant) && !noZero)
        {
            return shared_ptr<Expression>(new Variable());
        }
        else
        {
            return shared_ptr<Expression>(new Constant());
        }
    }
    // consider operators

    //trig functions with low probability
    if (RandomF() > 0.8f && !noZero)
    {
        // equal probability of cos and sin
        return RandomF() > 0.5f ? shared_ptr<Expression>(new Cos()) : shared_ptr<Expression>(new Sin());
    }
    float p = RandomF();
    //equal probabilty of binary opertaors
    if (p > (3.0f / 4.0f))
    {
        return shared_ptr<Expression>(new Plus());
    }
    else if (p > (2.0f / 4.0f))
    {
        return shared_ptr<Expression>(new Minus());
    }
    else if (p > (1.0f / 4.0f))
    {
        return shared_ptr<Expression>(new Multiply());
    }
    else
    {
        return shared_ptr<Expression>(new Divide());
    }
}

#define pointer_cast(T, U, p) shared_ptr<T>(new U(*dynamic_pointer_cast<U>(p)));

shared_ptr<Expression> Expression::Copy(const shared_ptr<Expression> &source)
{
    shared_ptr<Expression> exp;
    string typeName(typeid(*source).name());
    string prefix("class SymbolicRegression::");
    if (typeName == prefix + "Constant")
    {
        exp = pointer_cast(Expression, Constant, source); // shared_ptr<Expression>(new Constant(*dynamic_pointer_cast<Constant>(source)));
    }
    else if (typeName == prefix + "Variable")
    {
        exp = pointer_cast(Expression, Variable, source);
    }
    else if (typeName == prefix + "Plus")
    {
        exp = pointer_cast(Expression, Plus, source);
    }
    else if (typeName == prefix + "Minus")
    {
        exp = pointer_cast(Expression, Minus, source);
    }
    else if (typeName == prefix + "Multiply")
    {
        exp = pointer_cast(Expression, Multiply, source);
    }
    else if (typeName == prefix + "Divide")
    {
        exp = pointer_cast(Expression, Divide, source);
    }
    else if (typeName == prefix + "Cos")
    {
        exp = pointer_cast(Expression, Cos, source);
    }
    else if (typeName == prefix + "Sin")
    {
        exp = pointer_cast(Expression, Sin, source);
    }
    else
    {
        cout << "Error: Invalid type name " << typeName << endl;
    }
    return exp;
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
    lock_guard<mutex> lock(randMutex);
    return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (max - min) + min;
}

} // namespace SymbolicRegression