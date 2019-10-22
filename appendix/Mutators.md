## Mutators

ConstantMultiplierMutator.hpp

```cpp
#ifndef _CONSTANT_MULTIPLIER_MUTATOR_HPP_
#define _CONSTANT_MULTIPLIER_MUTATOR_HPP_
#include <memory>
#include "../../expression/Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class ConstantMultiplierMutator
{
public:
    static shared_ptr<Expression> Mutate(const shared_ptr<Expression> &exp);
};
} // namespace SymbolicRegression

#endif
```

ConstantMultiplierMutator.cpp

```cpp
#include "ConstantMultiplierMutator.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../expression/Constant.hpp"
#include "../../expression/Multiply.hpp"
#include "../../engine/Config.hpp"

namespace SymbolicRegression
{
using namespace std;

// TRY OUT SAME EXPRESSION WITH A DIFFERENT CONSTANT MULTIPLIER
shared_ptr<Expression> ConstantMultiplierMutator::Mutate(const shared_ptr<Expression> &exp)
{
    float k, prevFitness;
    auto tempExp = Expression::Copy(exp);
    prevFitness = tempExp->Fitness();
    vector<shared_ptr<Expression>>::iterator it;
    for (int i = 0; i < Config::GetInt("MutationRetries"); i++)
    {
        // Find subexpression of type constant
        it = find_if(tempExp->m_subexpressions.begin(),
                     tempExp->m_subexpressions.end(),
                     [](auto subexp) {
                         return EXPRESSION_TYPE(subexp) == CONSTANT_T;
                     });
        // Tree is already too tall, and don't already
        // have constants multiplied in top level
        // to be mutated
        if (tempExp->Depth() >= Config::GetInt("MaxDepth") &&
            it == tempExp->m_subexpressions.end())
        {
            break;
        }

        k = Expression::RandomF(-2, 2);
        auto f = [=](float x) { return k; };
        if (tempExp->Fitness(f) > prevFitness)
        {
            // If expression already has multiply expression in top level
            // and constant in second level
            if (EXPRESSION_TYPE(tempExp) == MULTIPLY_T &&
                it != tempExp->m_subexpressions.end())
            {
                auto constExp = shared_ptr<Expression>(new Constant(1, k));
                replace(tempExp->m_subexpressions.begin(),
                        tempExp->m_subexpressions.end(),
                        *it, constExp);
                prevFitness = tempExp->Fitness();
            }
            // Create new constant and multiplier expression
            else
            {
                auto constExp = shared_ptr<Expression>(new Constant(1, k));
                tempExp = shared_ptr<Expression>(new Multiply(0, constExp, tempExp));
                prevFitness = tempExp->Fitness();
            }
        }
    }

    return exp;
}
} // namespace SymbolicRegression

```

ConstantMutator.hpp

```cpp
#ifndef _CONSTANT_MUTATOR_HPP_
#define _CONSTANT_MUTATOR_HPP_
#include <memory>
#include "../../expression/Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class ConstantMutator
{
public:
    static shared_ptr<Expression> Mutate(const shared_ptr<Expression> &exp);
};
} // namespace SymbolicRegression

#endif
```

ConstantMutator.cpp

```cpp
#include "ConstantMutator.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../expression/Constant.hpp"
#include "../Solver.hpp"
#include "../../engine/Config.hpp"

namespace SymbolicRegression
{
using namespace std;
shared_ptr<Expression> ConstantMutator::Mutate(const shared_ptr<Expression> &exp)
{
    auto tempExp = Expression::Copy(exp);
    auto collapsedExp = tempExp->Collapse(tempExp);
    vector<shared_ptr<Expression>> constants;
    int i = int(Expression::RandomF(0, 100));
    copy_if(
        collapsedExp->begin(),
        collapsedExp->end(),
        back_inserter(constants),
        [](auto e) {
            return EXPRESSION_TYPE(e) == CONSTANT_T;
        });
    if (constants.size() == 0)
        return exp;
    i = i % constants.size();
    auto constantToMutate = dynamic_pointer_cast<Constant>(constants[i]);

    float k, bestK;
    float prevFitness = tempExp->Fitness();
    float testFitness = -1;
    for (int i = 0; i < Config::GetInt("MutationRetries"); i++)
    {
        auto f = [=](float x) { return k; };
        testFitness = tempExp->Fitness(f);
        if (testFitness > prevFitness || Expression::RandomF() < Solver::GetTemp())
        {
            prevFitness = testFitness;
            bestK = k;
        }
    }

    if (prevFitness > exp->Fitness() || Expression::RandomF() < Solver::GetTemp())
    {
        constantToMutate->m_k = bestK;
        return tempExp;
    }
    return exp;
}
} // namespace SymbolicRegression

```

SubexpressionMutator.hpp

```cpp
#ifndef _SUBEXPRESSION_MUTATOR_HPP_
#define _SUBEXPRESSION_MUTATOR_HPP_
#include <memory>
#include "../../expression/Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class SubexpressionMutator
{
public:
    static shared_ptr<Expression> Mutate(shared_ptr<Expression> exp);
};
} // namespace SymbolicRegression

#endif
```

SubexpressionMutator.cpp

```cpp
#include "SubexpressionMutator.hpp"
#include <vector>
#include <algorithm>
#include "../../engine/Config.hpp"
#include "../Solver.hpp"
namespace SymbolicRegression
{
using namespace std;

shared_ptr<Expression> SubexpressionMutator::Mutate(shared_ptr<Expression> exp)
{
    shared_ptr<Expression> bestMutation;
    for (int i = 0; i < Config::GetInt("MutationRetries"); i++)
    {
        auto tempExp = Expression::Copy(exp);
        auto collapsedExp = exp->Collapse(exp);
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

        // choose random operator
        auto expToChange = operators[int(Expression::RandomF(0, 100)) %
                                     operators.size()];

        // change a random subexpression of the operator
        int idx = int(Expression::RandomF(0, float(expToChange->Order() - 1)));
        expToChange->m_subexpressions[idx] =
            Expression::GenerateRandomExpression(expToChange->Level() + 1);
        if (!bestMutation || tempExp->Fitness() > bestMutation->Fitness())
        {
            bestMutation = tempExp;
        }
    }

    if (bestMutation->Fitness() > exp->Fitness() ||
        Expression::RandomF() < Solver::GetTemp())
    {
        return bestMutation;
    }
    return exp;
}
} // namespace SymbolicRegression

```

TrigMultiplierMutator.hpp

```cpp
#ifndef _TRIG_MULTIPLIER_MUTATOR_HPP_
#define _TRIG_MULTIPLIER_MUTATOR_HPP_
#include <memory>
#include "../../expression/Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class TrigMultiplierMutator
{
public:
    static shared_ptr<Expression> Mutate(const shared_ptr<Expression> &exp);
};
} // namespace SymbolicRegression

#endif
```

TrigMultiplierMutator.cpp

```cpp
#include "TrigMultiplierMutator.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../expression/Constant.hpp"
#include "../../expression/Variable.hpp"
#include "../../expression/Multiply.hpp"
#include "../../expression/Plus.hpp"
#include "../../expression/Sin.hpp"
#include "../../engine/Config.hpp"

namespace SymbolicRegression
{
using namespace std;
shared_ptr<Expression> TrigMultiplierMutator::Mutate(const shared_ptr<Expression> &exp)
{
    float frequency, displacement, output_frequency, output_displacement;
    float prevFitness, testFitness;
    prevFitness = exp->Fitness();
    for (int i = 0; i < Config::GetInt("MutationRetries"); i++)
    {
        if (exp->Depth() >= Config::GetInt("MaxDepth"))
        {
            break;
        }
        frequency = Expression::RandomF(-10, 10);
        displacement = Expression::RandomF(-10, 10);
        auto f = [=](float x) {
            return sinf(frequency * x + displacement);
        };
        testFitness = exp->Fitness(f);
        if (testFitness > prevFitness)
        {
            prevFitness = testFitness;
            output_frequency = frequency;
            output_displacement = displacement;
        }
    }

    if (prevFitness == exp->Fitness())
    {
        return exp;
    }
    auto frequencyConstant = shared_ptr<Expression>(new Constant(-1, output_frequency));
    auto displacementConstant = shared_ptr<Expression>(new Constant(-1, output_displacement));
    auto variable = shared_ptr<Expression>(new Variable(-1));

    auto variableTimesFrequency = shared_ptr<Expression>(new Multiply(0, frequencyConstant, variable));
    auto signalTerm = shared_ptr<Expression>(new Plus(0, variableTimesFrequency, displacementConstant));

    auto sinExpression = shared_ptr<Expression>(new Sin(0, signalTerm));
    return shared_ptr<Expression>(new Multiply(0, sinExpression, exp));
}
} // namespace SymbolicRegression

```

TruncateMutator.hpp

```cpp
#ifndef _TRUNCATE_MUTATOR_HPP_
#define _TRUNCATE_MUTATOR_HPP_
#include <memory>
#include "../../expression/Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class TruncateMutator
{
public:
    static shared_ptr<Expression> Mutate(const shared_ptr<Expression> &exp);
};
} // namespace SymbolicRegression

#endif
```

TruncateMutator.cpp

```cpp
#include "TruncateMutator.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../expression/Constant.hpp"
#include "../Solver.hpp"
#include <typeinfo>
#include "../../engine/Config.hpp"
namespace SymbolicRegression
{
using namespace std;
shared_ptr<Expression> TruncateMutator::Mutate(const shared_ptr<Expression> &exp)
{
    shared_ptr<Expression> bestMutation;
    for (int i = 0; i < Config::GetInt("MutationRetries"); i++)
    {
        auto tempExp = Expression::Copy(exp);
        auto collapsedExp = tempExp->Collapse(tempExp);
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
        auto opToTruncate = operators[int(Expression::RandomF(0, 100)) %
                                      operators.size()];
        opToTruncate->m_subexpressions[int(Expression::RandomF(0, 100)) %
                                       opToTruncate->m_subexpressions.size()] =
            Expression::GenerateRandomZeroOrderExpression(opToTruncate->Level() + 1);
        if (!bestMutation || tempExp->Fitness() > bestMutation->Fitness())
        {
            bestMutation = tempExp;
        }
    }

    if (bestMutation->Fitness() > exp->Fitness() ||
        Expression::RandomF() < Solver::GetTemp())
    {
        return bestMutation;
    }
    return exp;
}
} // namespace SymbolicRegression


```
