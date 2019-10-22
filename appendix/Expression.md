## Expression

Expression.hpp

```cpp
#ifndef _EXPRESSION_HPP_
#define _EXPRESSION_HPP_
#include <functional>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <algorithm>
#include <mutex>
#include <typeinfo>
namespace SymbolicRegression
{
using namespace std;
class Expression
{
public:
    // Factory Functions
    shared_ptr<Expression> static GenerateRandomExpression(int level, bool noConstant = false, bool noZero = false, bool noTrig = false);
    shared_ptr<Expression> static GenerateRandomZeroOrderExpression(int level);
    shared_ptr<Expression> static GenerateRandomTrigExpression(int level);
    shared_ptr<Expression> static GenerateRandomBinaryOperator(int level);

    friend class SubexpressionMutator;
    friend class TruncateMutator;
    friend class ConstantMultiplierMutator;
    friend class TrigMultiplierMutator;
    friend class CrossoverMutatorReproducer;

public:
    /*
        STATIC HELPER FUNCTIONS
    */
    static function<bool(const shared_ptr<Expression> &, const shared_ptr<Expression> &)> FitnessComparer;

    static shared_ptr<Expression> Initialize(shared_ptr<Expression> self, shared_ptr<Expression> parent);

    shared_ptr<vector<shared_ptr<Expression>>> Collapse(const shared_ptr<Expression> &self) const;
    static shared_ptr<Expression> Simplify(shared_ptr<Expression> exp);

    static float RandomF();
    static float RandomF(float min, float max);
    inline virtual string ToString() const = 0;
    static shared_ptr<Expression> Copy(const shared_ptr<Expression> &source);

public:
    /*
        PUBLIC ACCESSORS
    */
    virtual function<float(float)> ToFunction() const;

    float Fitness();
    float Fitness(function<float(float)> f);
    float CalculateFitness(function<float(float)> f) const;

    inline int Order() const { return m_order; }
    inline int Level() const { return m_level; }
    int Depth() const;
    static bool IsValid(shared_ptr<Expression> exp);

public:
    /*
    Diversity
    */
    static float Diversity(const shared_ptr<Expression> &e1, const shared_ptr<Expression> &e2);

public:
    /*
        EXPRESSION PREDICATES TO HELP WITH EXPRESSION SIMPLIFICATION
    */
    typedef function<bool(const shared_ptr<Expression> &)> ExpressionPredicate;
    static ExpressionPredicate evaluatesToConstant;
    static ExpressionPredicate subexpressionsCancelOut;
    static ExpressionPredicate all;
    static ExpressionPredicate isTrigFunction;
    static ExpressionPredicate minusOrDivide;
    static function<float(float)> one;
#define EXPRESSION_TYPE(exp) string(typeid(*exp).name())
#define CONSTANT_T string("class SymbolicRegression::Constant")
#define VARIABLE_T string("class SymbolicRegression::Variable")
#define SIN_T string("class SymbolicRegression::Sin")
#define COS_T string("class SymbolicRegression::Cos")
#define PLUS_T string("class SymbolicRegression::Plus")
#define MINUS_T string("class SymbolicRegression::Minus")
#define DIVIDE_T string("class SymbolicRegression::Divide")
#define MULTIPLY_T string("class SymbolicRegression::Multiply")

protected:
    Expression(int level);
    Expression(const Expression &other);

    inline static vector<shared_ptr<Expression>>::iterator FindFirst(
        vector<shared_ptr<Expression>> &operators,
        ExpressionPredicate subexpPredicate,
        ExpressionPredicate opPredicate)
    {
        return find_if(operators.begin(), operators.end(), [&](auto op) {
            return opPredicate(op) && all_of(op->m_subexpressions.begin(), op->m_subexpressions.end(), subexpPredicate);
        });
    }

    inline static bool AnySubExpression(shared_ptr<Expression> exp, ExpressionPredicate pred)
    {
        return pred(exp) || any_of(
                                exp->m_subexpressions.begin(),
                                exp->m_subexpressions.end(), [&](auto subexp) {
                                    return AnySubExpression(subexp, pred);
                                });
    }

    // replaces e1 with e2 in e1's original expression tree

    inline static void ReplaceExpression(shared_ptr<Expression> e1, shared_ptr<Expression> e2)
    {
        if (!e1->m_parent.expired())
        {
            auto parent = e1->m_parent.lock();
            for (int i = 0; i < parent->m_subexpressions.size(); i++)
            {
                if (parent->m_subexpressions[i]->ToString() == e1->ToString())
                {
                    parent->m_subexpressions[i] = e2;
                }
            }
        }
    }

    inline void RecalculateLevels(int level)
    {
        m_level = level;
        for_each(m_subexpressions.begin(), m_subexpressions.end(), [&](auto subexp) {
            subexp->RecalculateLevels(level + 1);
        });
    }

protected:
    int m_order = -1;                  // How many parameters the current expression needs
    function<float(float)> m_func = 0; //function presenting this expression node's function
    weak_ptr<Expression> m_parent;
    vector<shared_ptr<Expression>> m_subexpressions;
    float m_fitness = -1;
    int m_level = -1;

private:
    static mutex randMutex;
};
} // namespace SymbolicRegression
// ostream &operator<<(ostream &os, const Expression &e)
// {
//     os << e.ToString();
//     return os;
// }
#endif

```

Expression.cpp

```cpp
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
```

Constant.hpp

```cpp
#ifndef _CONSTANT_HPP_
#define _CONSTANT_HPP_
#include "Expression.hpp"
#include <string>
#include <iomanip>
#include <sstream>
namespace SymbolicRegression
{
class Constant : public Expression
{
public:
    friend class ConstantMultiplierMutator;
    inline Constant(int level) : Expression(level)
    {
        m_k = SymbolicRegression::Expression::RandomF(-10, 10);
        m_func = [&](float x) { return m_k; };
        m_order = 0;
    }
    inline Constant(int level, float k) : Expression(level)
    {
        m_k = k;
        m_func = [&](float x) { return m_k; };
        m_order = 0;
    }
    inline Constant(const Constant &other) : Expression(other)
    {
        m_k = other.m_k;
        m_func = [&](float x) { return m_k; };
        m_order = 0;
    };
    inline virtual std::string ToString() const override
    {
        stringstream s;
        s << fixed << setprecision(3) << m_k;
        return s.str();
    }
    friend class ConstantMutator;

protected:
    float m_k; // value of constant
};
} // namespace SymbolicRegression
#endif


```

Divide.hpp

```cpp
#ifndef _DIVIDE_HPP_
#define _DIVIDE_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Divide : public Expression
{
public:
    inline Divide(int level) : Expression(level)
    {
        if (level + 1 == Config::GetInt("MaxDepth"))
        {
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
        }
        else
        {
            m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression(level + 1)));
            m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression(level + 1, false, true)));
        }
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) / m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }

    inline Divide(const Divide &other) : Expression(other)
    {
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) / m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }
    inline virtual std::string ToString() const override
    {
        return m_subexpressions[0]->ToString() + " / " + m_subexpressions[1]->ToString();
    }
};
} // namespace SymbolicRegression
#endif
```

Minus.hpp

```cpp
#ifndef _MINUS_HPP_
#define _MINUS_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Minus : public Expression
{
public:
    inline Minus(int level) : Expression(level)
    {
        if (level + 1 == Config::GetInt("MaxDepth"))
        {
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
        }
        else
        {
            m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression(level + 1)));
            m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression(level + 1)));
        }
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
```

Plus.hpp

```cpp
#ifndef _PLUS_HPP_
#define _PLUS_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class Plus : public Expression
{
public:
    inline Plus(int level) : Expression(level)
    {
        if (level + 1 == Config::GetInt("MaxDepth"))
        {
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
        }
        else
        {
            m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression(level + 1)));
            m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression(level + 1)));
        }
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) + m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }

    inline Plus(int level, shared_ptr<Expression> e1, shared_ptr<Expression> e2) : Expression(level)
    {
        m_subexpressions.push_back(e1);
        m_subexpressions.push_back(e2);

        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) + m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }

    inline Plus(const Plus &other) : Expression(other)
    {
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) + m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    };
    inline virtual string ToString() const override
    {
        string tmp = m_subexpressions[1]->ToString();
        if (m_subexpressions[1]->ToString().at(0) == '-')
            return "(" + m_subexpressions[0]->ToString() + " - " + tmp.substr(1, tmp.length()) + ")";
        return "(" + m_subexpressions[0]->ToString() + " + " + m_subexpressions[1]->ToString() + ")";
    }
};
} // namespace SymbolicRegression
#endif
```

Multiply.hpp

```cpp
#ifndef _MULTIPLY_HPP_
#define _MULTIPLY_HPP_
#include "Expression.hpp"
#include "../engine/Config.hpp"

namespace SymbolicRegression
{
using namespace std;
class Multiply : public Expression
{
public:
    inline Multiply(int level) : Expression(level)
    {
        if (level + 1 == Config::GetInt("MaxDepth"))
        {
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
        }
        else
        {
            m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression(level + 1)));
            m_subexpressions.push_back(std::shared_ptr<Expression>(Expression::GenerateRandomExpression(level + 1)));
        }
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) * m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }

    inline Multiply(int level, shared_ptr<Expression> e1, shared_ptr<Expression> e2) : Expression(level)
    {

        m_subexpressions.push_back(e1);
        m_subexpressions.push_back(e2);
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) * m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }

    inline Multiply(const Multiply &other) : Expression(other)
    {
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) * m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    }

    inline virtual string ToString() const override
    {
        return m_subexpressions[0]->ToString() + " * " + m_subexpressions[1]->ToString();
    }
};
} // namespace SymbolicRegression
#endif
```

Sin.hpp

```cpp
#ifndef _SIN_HPP_
#define _SIN_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Sin : public Expression
{
public:
    inline Sin(int level) : Expression(level)
    {
        if (level + 1 == Config::GetInt("MaxDepth"))
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
        else
            m_subexpressions.push_back(Expression::GenerateRandomExpression(level + 1, true, false, true));
        m_func = [&](float x) {
            return sinf(m_subexpressions[0]->ToFunction()(x));
        };
        m_order = 1;
    }

    inline Sin(int level, shared_ptr<Expression> e) : Expression(level)
    {
        m_subexpressions.push_back(e);
        m_func = [&](float x) {
            return sinf(m_subexpressions[0]->ToFunction()(x));
        };
        m_order = 1;
    }

    inline Sin(const Sin &other) : Expression(other)
    {
        m_func = [&](float x) {
            return sinf(m_subexpressions[0]->ToFunction()(x));
        };
        m_order = 1;
    };

    inline virtual std::string ToString() const override
    {
        return "sin(" + m_subexpressions[0]->ToString() + ")";
    }
};
} // namespace SymbolicRegression
#endif
```

Cos.hpp

```cpp
#ifndef _COS_HPP_
#define _COS_HPP_
#include "Expression.hpp"
#include <math.h>
#include "../engine/Config.hpp"
namespace SymbolicRegression
{
class Cos : public Expression
{
public:
    inline Cos(int level) : Expression(level)
    {
        if (level + 1 == Config::GetInt("MaxDepth"))
            m_subexpressions.push_back(Expression::GenerateRandomZeroOrderExpression(level + 1));
        else
            m_subexpressions.push_back(Expression::GenerateRandomExpression(level + 1, true, false, true));
        m_func = [&](float x) {
            return cosf(m_subexpressions[0]->ToFunction()(x));
        };
        m_order = 1;
    }

    inline Cos(const Cos &other) : Expression(other)
    {
        m_func = [&](float x) {
            return cosf(m_subexpressions[0]->ToFunction()(x));
        };
        m_order = 1;
    }

    inline virtual std::string ToString() const override
    {
        return "cos(" + m_subexpressions[0]->ToString() + ")";
    }
};
} // namespace SymbolicRegression
#endif
```

Variable.hpp

```cpp
#ifndef _VARIABLE_HPP_
#define _VARIABLE_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Variable : public Expression
{
public:
    inline Variable(int level) : Expression(level)
    {
        m_func = [&](float x) {
            return x;
        };
        m_order = 0;
        m_subexpressions.clear();
    }

    inline Variable(const Variable &other) : Expression(other)
    {
        m_func = [&](float x) {
            return x;
        };
        m_order = 0;
        m_subexpressions.clear();
    }

    inline virtual std::string ToString() const override
    {
        return "x";
    }
};
} // namespace SymbolicRegression
#endif
```
