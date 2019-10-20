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
#include "../engine/OutputLogger.hpp"
#include <typeinfo>
#include "../genetic-programming/Solver.hpp"
#include <chrono>
#include <thread>
namespace SymbolicRegression
{
using namespace std;

#pragma region Static Variables
function<bool(
    const shared_ptr<Expression> &,
    const shared_ptr<Expression> &)>
    Expression::FitnessComparer = [](const shared_ptr<Expression> &a,
                                     const shared_ptr<Expression> &b) -> bool {
    return a->Fitness() > b->Fitness();
};

Expression::ExpressionPredicate Expression::evaluatesToConstant = [](auto subexpression) {
    auto f = subexpression->ToFunction();
    float f0 = f(0);
    float f1 = f(1);
    float f2 = f(2);
    if (abs(f0 - f1) < 0.1f && abs(f2 - f1) < 0.1f)
        return true;
    else
        return false;
};

Expression::ExpressionPredicate Expression::subexpressionsCancelOut = [](auto subexpression) {
    return EXPRESSION_TYPE(subexpression) == VARIABLE_T;
};

Expression::ExpressionPredicate Expression::all = [](auto op) {
    return true;
};

Expression::ExpressionPredicate Expression::minusOrDivide = [&](auto op) {
    auto typeStr = EXPRESSION_TYPE(op);
    return typeStr == DIVIDE_T ||
           typeStr == MINUS_T;
};

Expression::ExpressionPredicate Expression::isTrigFunction = [&](auto op) {
    auto typeStr = EXPRESSION_TYPE(op);
    return typeStr == SIN_T ||
           typeStr == SIN_T;
};

function<float(float)> Expression::one = [](float x) { return 1.0f; };
mutex Expression::randMutex;
#pragma endregion

Expression::Expression(int level)
{
    m_func = 0;
    m_order = -1;
    m_level = level;
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
}

float Expression::Fitness()
{
    if (m_fitness == -1)
    {
        m_fitness = Fitness(one);
    }
    return m_fitness;
}

float Expression::Fitness(function<float(float)> f)
{
    return CalculateFitness(f);
}

float Expression::CalculateFitness(function<float(float)> f2) const
{
    using namespace std;
    function<float(float)> f = ToFunction();
    float AbsoluteErrorSum = 0;
    for_each(Config::Data->begin(), Config::Data->end(), [&](tuple<float, float> x) {
        AbsoluteErrorSum += abs(f(get<0>(x)) * f2(get<0>(x)) - get<1>(x));
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
    return self;
}

static void CountSubexpressionTypes(const shared_ptr<Expression> &e, vector<int> &results)
{
    results.clear();
    for (int i = 0; i < 8; i++)
    {
        results.push_back(0);
    }
    auto collapsed = e->Collapse(e);
    for_each(collapsed->begin(), collapsed->end(), [&](auto subexp) {
        if (EXPRESSION_TYPE(subexp) == CONSTANT_T)
        {
            results[0] += 1;
        }
        else if (EXPRESSION_TYPE(subexp) == VARIABLE_T)
        {
            results[1] += 1;
        }
        else if (EXPRESSION_TYPE(subexp) == SIN_T)
        {
            results[2] += 1;
        }
        else if (EXPRESSION_TYPE(subexp) == COS_T)
        {
            results[3] += 1;
        }
        else if (EXPRESSION_TYPE(subexp) == PLUS_T)
        {
            results[4] += 1;
        }
        else if (EXPRESSION_TYPE(subexp) == MINUS_T)
        {
            results[5] += 1;
        }
        else if (EXPRESSION_TYPE(subexp) == DIVIDE_T)
        {
            results[6] += 1;
        }
        else if (EXPRESSION_TYPE(subexp) == MULTIPLY_T)
        {
            results[7] += 1;
        }
    });
}

float Expression::Diversity(const shared_ptr<Expression> &e1, const shared_ptr<Expression> &e2)
{
    vector<int> e1subexp, e2subexp;
    CountSubexpressionTypes(e1, e1subexp);
    CountSubexpressionTypes(e2, e2subexp);
    float diff = 0;
    for (int i = 0; i < e1subexp.size(); i++)
    {
        diff += abs(e1subexp[i] - e2subexp[i]);
    }
    return diff;
}

shared_ptr<Expression> Expression::Simplify(shared_ptr<Expression> exp)
{
    if (exp->Depth() == 1)
    {
        return exp;
    }
    while (true)
    {
        exp = Initialize(exp, nullptr);
        exp->RecalculateLevels(0);

        // 1. find all operators
        auto collapsedExp = exp->Collapse(exp);
        if (collapsedExp->size() == 1)
            return exp;

        vector<shared_ptr<Expression>> operators;
        copy_if(collapsedExp->begin(), collapsedExp->end(), back_inserter(operators),
                [](auto e) { return e->Order() > 0; });
        if (operators.size() == 0)
            return exp;

        // 2. check for expressions that cancels out the variables
        vector<shared_ptr<Expression>>::iterator it;
        if ((it = FindFirst(operators, subexpressionsCancelOut, minusOrDivide)) != operators.end())
        {
            auto expToSimplify = (*it);
            auto replacementConstant = shared_ptr<Expression>(new Constant(expToSimplify->Level(), RandomF(0.0f, 0.2f)));
            // no parent
            if (expToSimplify->Level() == 0)
            {
                return replacementConstant;
            }
            else
            {
                ReplaceExpression(expToSimplify, replacementConstant);
            }
        }
        // 3. check for expressions that evaluates to constants
        else if ((it = FindFirst(operators, all, evaluatesToConstant)) != operators.end())
        {
            auto expToSimplify = (*it);

            // replace with constant at same level and of the same size
            auto replacementConstant = shared_ptr<Expression>(
                new Constant(
                    (*it)->Level(),
                    expToSimplify->ToFunction()(1)));
            //no parent
            if (expToSimplify->Level() == 0)
            {
                return replacementConstant;
            }
            else
            {
                ReplaceExpression(expToSimplify, replacementConstant);
            }
        }
        // 4. check for nested trig functions
        else if ((it = FindFirst(operators, isTrigFunction, [](auto subexp) {
                      return AnySubExpression(subexp, isTrigFunction);
                  })) != operators.end())
        {
            auto expToSimplify = (*it);

            // replace with constant at same level and of the same size
            auto randConstExpression = shared_ptr<Expression>(
                new Constant(expToSimplify->Level()));
            auto variableExprssion = shared_ptr<Expression>(
                new Variable(expToSimplify->Level()));
            auto replacementExp = shared_ptr<Expression>(
                new Multiply((*it)->Level(), randConstExpression, variableExprssion));
            //no parent
            if (expToSimplify->Level() == 0)
            {
                return replacementExp;
            }
            else
            {
                ReplaceExpression(expToSimplify, replacementExp);
            }
        }
        else
        {
            return exp;
        }
    }
} // namespace SymbolicRegression

bool Expression::IsValid(shared_ptr<Expression> exp)
{
    auto f = exp->ToFunction();
    if (isnan(f(0)))
        return false;
    else if (EXPRESSION_TYPE(exp) == CONSTANT_T)
        return false;
    else if (exp->Depth() > Config::GetInt("MaxDepth"))
        return false;
    return true;
}

shared_ptr<vector<shared_ptr<Expression>>> Expression::Collapse(const shared_ptr<Expression> &self) const
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
        return shared_ptr<Expression>(new Variable(level));
    }
    else
    {
        return shared_ptr<Expression>(new Constant(level));
    }
}

shared_ptr<Expression> Expression::GenerateRandomBinaryOperator(int level)
{
    float p = RandomF();
    //equal probabilty of binary operators
    if (p > (3.0f / 4.0f))
    {
        return shared_ptr<Expression>(new Plus(level));
    }
    else if (p > (2.0f / 4.0f))
    {
        return shared_ptr<Expression>(new Minus(level));
    }
    else if (p > (1.0f / 4.0f))
    {
        return shared_ptr<Expression>(new Multiply(level));
    }
    else
    {
        return shared_ptr<Expression>(new Divide(level));
    }
}

shared_ptr<Expression> Expression::GenerateRandomTrigExpression(int level)
{
    // equal probability of cos and sin
    if (RandomF() > 0.5f)
    {
        return shared_ptr<Expression>(new Cos(level));
    }
    else
    {
        return shared_ptr<Expression>(new Sin(level));
    }
}

shared_ptr<Expression> Expression::GenerateRandomExpression(int level, bool noConstant, bool noZero, bool noTrig)
{
    shared_ptr<Expression> exp;

    if ((RandomF() > 0.5f || level >= Config::GetInt("MaxDepth") - 1) && level > 0)
    {
        // prioritize constants
        exp = GenerateRandomZeroOrderExpression(level);
    }
    else if (RandomF() > 0.8f && !noZero && !noTrig)
    {
        //trig functions with low probability
        exp = GenerateRandomTrigExpression(level);
    }
    else
    {
        exp = GenerateRandomBinaryOperator(level);
    }
    return Simplify(exp);
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

void Expression::Random(int min, int max, int count, vector<int> &output)
{
    while (output.size() < count)
    {
        if (min == max)
            output.push_back(min);
        else
        {
            int next = (int)RandomF((float)min, (float)max);
            if (!any_of(output.begin(), output.end(), [=](auto x) {
                    return next == x;
                }))
            {
                output.push_back(next);
            }
        }
    }
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