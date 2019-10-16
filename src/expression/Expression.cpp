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
#include "../genetic-programming/Solver.hpp"
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

Expression::Expression(int level)
{
    m_func = 0;
    m_order = -1;
    m_level = level;
    cout << "Expression::Expression(int level)" << endl;
}

Expression::Expression(const Expression &other)
{
    m_order = other.m_order;
    m_func = 0;
    m_level = other.m_level;
    for (int i = 0; i < other.m_subexpressions.size(); i++)
    {
        m_subexpressions.push_back(Copy(other.m_subexpressions[i]));
    }
    cout << "Expression::Expression(const Expression &other)" << endl;
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

shared_ptr<Expression> Expression::Initialize(shared_ptr<Expression> self, shared_ptr<Expression> parent)
{
    if (parent != nullptr)
        self->m_parent = parent;
    for_each(self->m_subexpressions.begin(), self->m_subexpressions.end(), [&](const shared_ptr<Expression> &subexp) {
        subexp->Initialize(subexp, self);
    });
    return Simplify(self);
}

shared_ptr<Expression> Expression::Simplify(shared_ptr<Expression> exp)
{
    // int count = 0;
    while (true)
    {
        // find all operators
        auto collapsedExp = exp->Collapse(exp);
        if (collapsedExp->size() == 1)
            return exp;
        vector<shared_ptr<Expression>> operators;
        copy_if(
            collapsedExp->begin(),
            collapsedExp->end(),
            back_inserter(operators),
            [](auto e) {
                return e->Order() > 0;
            });
        if (operators.size() == 0)
            return exp;
        // cout << string("simplifying " + exp->Depth() + string(" : ") + exp->ToString() + " for the " + to_string(count++)) << endl;
        vector<shared_ptr<Expression>>::iterator it;
        if ((it = find_if(operators.begin(), operators.end(), [](auto op) {
                 return all_of(op->m_subexpressions.begin(), op->m_subexpressions.end(), [&](auto subexpression) {
                     return string(typeid(*subexpression).name()) == string("class SymbolicRegression::Constant");
                 });
             })) != operators.end())
        {
            // replace this node

            float val = (*it)->ToFunction()(0);
            auto replacementConstant = shared_ptr<Expression>(new Constant((*it)->Level(), val));
            //no parent
            if (!(*it)->m_parent)
            {
                return replacementConstant;
            }
            auto parent = (*it)->m_parent;
            for (int i = 0; i < parent->m_subexpressions.size(); i++)
            {
                if (string(parent->m_subexpressions[i]->ToString()) == (*it)->ToString())
                {
                    parent->m_subexpressions[i] = replacementConstant;
                }
            }
        }
        else if ((it = find_if(operators.begin(), operators.end(), [](auto op) {
                      return all_of(op->m_subexpressions.begin(), op->m_subexpressions.end(), [&](auto subexpression) {
                          return string(typeid(*subexpression).name()) == string("class SymbolicRegression::Variable");
                      });
                  })) != operators.end())
        {
            auto replacementConstant = shared_ptr<Expression>(new Constant((*it)->Level(), RandomF(0.0f, 0.2f)));
            // no parent
            if (!(*it)->m_parent)
            {
                return replacementConstant;
            }
            auto parent = (*it)->m_parent;
            for (int i = 0; i < parent->m_subexpressions.size(); i++)
            {
                if (string(parent->m_subexpressions[i]->ToString()) == (*it)->ToString())
                {
                    parent->m_subexpressions[i] = replacementConstant;
                }
            }
        }
        else
        {
            return exp;
        }
    }
}

bool Expression::IsValid(shared_ptr<Expression> exp)
{
    string prefix = "class SymbolicRegression::";
    auto collapsedSubExp = exp->Collapse(exp);
    vector<shared_ptr<Expression>> divides;
    copy_if(
        collapsedSubExp->begin(),
        collapsedSubExp->end(),
        back_inserter(divides),
        [&](auto e) {
            return string(typeid(*e).name()) == string(prefix + "Divide");
        });
    if (divides.size() == 0)
        return true;
    bool valid = !any_of(divides.begin(), divides.end(), [&](const shared_ptr<Expression> &divide) {
        string typeName = typeid(*(divide->m_subexpressions[1])).name();
        return typeName == (prefix + "Variable") ||
               typeName == (prefix + "Sin") ||
               typeName == (prefix + "Cos");
    });
    return valid;
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

shared_ptr<Expression> Expression::GenerateRandomZeroOrderExpression(int level)
{
    if (RandomF() > 0.5f)
    {
        return Initialize(shared_ptr<Expression>(new Variable(level)), nullptr);
    }
    else
    {
        return Initialize(shared_ptr<Expression>(new Constant(level)), nullptr);
    }
}

shared_ptr<Expression> Expression::GenerateRandomBinaryOperator(int level)
{
    float p = RandomF();
    //equal probabilty of binary operators
    if (p > (3.0f / 4.0f))
    {
        return Initialize(shared_ptr<Expression>(new Plus(level)), nullptr);
    }
    else if (p > (2.0f / 4.0f))
    {
        return Initialize(shared_ptr<Expression>(new Minus(level)), nullptr);
    }
    else if (p > (1.0f / 4.0f))
    {
        return Initialize(shared_ptr<Expression>(new Multiply(level)), nullptr);
    }
    else
    {
        return Initialize(shared_ptr<Expression>(new Divide(level)), nullptr);
    }
}

shared_ptr<Expression> Expression::GenerateRandomTrigExpression(int level)
{
    // equal probability of cos and sin
    if (RandomF() > 0.5f)
    {
        return Initialize(shared_ptr<Expression>(new Cos(level)), nullptr);
    }
    else
    {
        return Initialize(shared_ptr<Expression>(new Sin(level)), nullptr);
    }
}

shared_ptr<Expression> Expression::GenerateRandomExpression(int level, bool noConstant, bool noZero, bool noTrig)
{
    // prioritize constants
    if (RandomF() > 0.5f || level >= Config::GetInt("MaxDepth") - 1)
    {
        return GenerateRandomZeroOrderExpression(level);
    }
    //trig functions with low probability
    if (RandomF() > 0.8f && !noZero && !noTrig)
    {
        GenerateRandomTrigExpression(level);
    }
    return GenerateRandomBinaryOperator(level);
}

#define pointer_cast(T, U, p) shared_ptr<T>(new U(*dynamic_pointer_cast<U>(p)));

shared_ptr<Expression> Expression::Copy(const shared_ptr<Expression> &source)
{
    shared_ptr<Expression> exp;
    string typeName(typeid(*source).name());
    string prefix("class SymbolicRegression::");
    if (typeName == prefix + "Constant")
    {
        exp = pointer_cast(Expression, Constant, source);
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
    exp = Initialize(exp, nullptr);
    return exp;
}

function<float(float)> Expression::ToFunction() const
{
    return m_func;
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

int Expression::Depth() const
{
    if (m_order == 0)
        return 1;
    int childDepth = -1;
    for (int i = 0; i < m_subexpressions.size(); i++)
    {
        childDepth = max(childDepth, m_subexpressions[i]->Depth());
    }
    return 1 + childDepth;
}

} // namespace SymbolicRegression