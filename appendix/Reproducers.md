## Reproducers

AsexualReproducer.hpp

```cpp
#ifndef _ASEXUAL_REPRODUCER_HPP_
#define _ASEXUAL_REPRODUCER_HPP_
#include "Reproducer.hpp"
namespace SymbolicRegression
{
using namespace std;
class AsexualReproducer : public Reproducer
{
public:
    inline AsexualReproducer(int populationCount) : Reproducer(populationCount){};
    virtual shared_ptr<list<shared_ptr<Expression>>> Reproduce(
        const list<shared_ptr<Expression>> &parents) override;
    inline shared_ptr<Expression> CreateOffspring(const shared_ptr<Expression> p1, const shared_ptr<Expression> p2) override
    {
        return Expression::GenerateRandomExpression(0, true);
    }
};
} // namespace SymbolicRegression

#endif
```

AsexualReproducer.cpp

```cpp
#include "AsexualReproducer.hpp"
#include "../Solver.hpp"
#include "../mutators/ConstantMultiplierMutator.hpp"
#include "../mutators/ConstantMutator.hpp"
#include "../mutators/SubexpressionMutator.hpp"
#include "../mutators/TrigMultiplierMutator.hpp"
#include "../mutators/TruncateMutator.hpp"
namespace SymbolicRegression
{
shared_ptr<list<shared_ptr<Expression>>> AsexualReproducer::Reproduce(
    const list<shared_ptr<Expression>> &parents)
{
    m_offsprings.clear();
    int parentCount = int(parents.size());
    while (m_offsprings.size() < m_populationCount)
    {
        int idx = int(Expression::RandomF(0.0f, float(m_populationCount - 1)));
        auto parent = parents.begin();
        advance(parent, idx);
        shared_ptr<Expression> offspring;
        int attempts = 0;
        do
        {
            offspring = Expression::Copy(*parent);
            offspring = ConstantMutator::Mutate(offspring);
            if (Expression::RandomF() > 0.7f)
            {
                offspring = ConstantMultiplierMutator::Mutate(offspring);
            }
            if (Expression::RandomF() > 0.7f)
            {
                offspring = TrigMultiplierMutator::Mutate(offspring);
            }
            if (Expression::RandomF() > 0.7f)
            {
                offspring = SubexpressionMutator::Mutate(offspring);
            }
            if (Expression::RandomF() > 0.7f)
            {
                offspring = TruncateMutator::Mutate(offspring);
            }
            attempts += 1;
            if (attempts > 1000)
            {
                m_offsprings.insert(make_pair((*parent)->ToString(), *parent));
                break;
            }
        } while (offspring->Fitness() < (*parent)->Fitness() ||
                 Expression::RandomF() < Solver::GetTemp());
        if (Expression::IsValid(offspring) && m_offsprings.find(offspring->ToString()) == m_offsprings.end())
        {
            m_offsprings.insert(make_pair(offspring->ToString(), offspring));
        }
    }
    shared_ptr<list<shared_ptr<Expression>>> output(new list<shared_ptr<Expression>>(m_offsprings.size()));
    transform(m_offsprings.begin(), m_offsprings.end(), output->begin(), [](auto p) {
        return p.second;
    });
    return output;
}
} // namespace SymbolicRegression
```

CrossoverMutatorReproducer.hpp

```cpp
#ifndef _CROSSOVER_MUTATOR_REPRODUCER_HPP_
#define _CROSSOVER_MUTATOR_REPRODUCER_HPP_
#include "Reproducer.hpp"
namespace SymbolicRegression
{
using namespace std;
class CrossoverMutatorReproducer : public Reproducer
{

public:
    inline CrossoverMutatorReproducer(int populationCount) : Reproducer(populationCount){};
    virtual shared_ptr<Expression> CreateOffspring(const shared_ptr<Expression> p1, const shared_ptr<Expression> p2) override;

private:
    shared_ptr<Expression> *FindCrossoverPoint(shared_ptr<Expression> root, int maxDepth);
};
} // namespace SymbolicRegression

#endif
```

CrossoverMutatorReproducer.cpp

```cpp
#include "CrossoverMutatorReproducer.hpp"
#include <memory>
#include "../../expression/Expression.hpp"
#include "../mutators/ConstantMutator.hpp"
#include "../mutators/SubexpressionMutator.hpp"
#include "../mutators/ConstantMultiplierMutator.hpp"
#include "../mutators/TrigMultiplierMutator.hpp"
#include "../mutators/TruncateMutator.hpp"
#include <algorithm>
#include <iostream>
#include "../../engine/Config.hpp"
#include "../Solver.hpp"
namespace SymbolicRegression
{
using namespace std;

shared_ptr<Expression> *CrossoverMutatorReproducer::FindCrossoverPoint(shared_ptr<Expression> root, int maxDepth)
{
    auto collapsedSubExp = root->Collapse(root);
    vector<shared_ptr<Expression>> operators;
    copy_if(
        collapsedSubExp->begin(),
        collapsedSubExp->end(),
        back_inserter(operators),
        [&](auto e) {
            return e->Order() > 0;
        });
    if (operators.size() == 0)
        return nullptr;
    auto expToChange = operators[int(Expression::RandomF(0, 100)) % operators.size()];
    return &(expToChange->m_subexpressions[int(Expression::RandomF(0, float(expToChange->Order()) - 1.0f))]);
}

shared_ptr<Expression> CrossoverMutatorReproducer::CreateOffspring(const shared_ptr<Expression> p1, const shared_ptr<Expression> p2)
{
    shared_ptr<Expression> child;
    shared_ptr<Expression> bestChildSoFar;
    int attempts = 0;
    do
    {
        child = Expression::Copy(p1);
        // Not tall enough to crossover
        if (child->Depth() == 1 || p2->Depth() == 1)
        {
            return child;
        }

        // Find a random cross over point
        auto *cross1 = FindCrossoverPoint(child, Config::GetInt("MaxDepth"));
        auto *cross2 = FindCrossoverPoint(p2, (*cross1)->Depth());
        if (cross1 != nullptr && cross2 != nullptr)
        {
            *cross1 = Expression::Copy(*cross2);
        }
        child = ConstantMutator::Mutate(child);
        if (Expression::RandomF() > 0.8f)
        {
            child = ConstantMultiplierMutator::Mutate(child);
        }
        if (Expression::RandomF() > 0.8f)
        {
            child = TrigMultiplierMutator::Mutate(child);
        }
        if (Expression::RandomF() > 0.8f)
        {
            child = SubexpressionMutator::Mutate(child);
        }
        if (Expression::RandomF() > 0.8f)
        {
            child = TruncateMutator::Mutate(child);
        }
        child = Expression::Simplify(child);

        if (Expression::RandomF() < Solver::GetTemp())
        {
            return child;
        }
        if (!bestChildSoFar || child->Fitness() > bestChildSoFar->Fitness())
        {
            bestChildSoFar = child;
        }
        attempts += 1;
        if (attempts > 100)
        {
            return bestChildSoFar;
        }
    } while (bestChildSoFar->Fitness() < p1->Fitness() &&
             bestChildSoFar->Fitness() < p2->Fitness());
    return bestChildSoFar;
}

} // namespace SymbolicRegression
```

MutatorReproducer.hpp

```cpp
#ifndef _MUTATOR_REPRODUCER_HPP_
#define _MUTATOR_REPRODUCER_HPP_
#include "Reproducer.hpp"
namespace SymbolicRegression
{
using namespace std;
class MutatorReproducer : public Reproducer
{
public:
    inline MutatorReproducer(int populationCount) : Reproducer(populationCount){};
    virtual shared_ptr<Expression> CreateOffspring(const shared_ptr<Expression> p1, const shared_ptr<Expression> p2) override;
};
} // namespace SymbolicRegression

#endif
```

MutatorReproducer.cpp

```cpp
#include "RandomReproducer.hpp"
#include <algorithm>
#include "MutatorReproducer.hpp"
#include <iostream>
#include "../mutators/ConstantMutator.hpp"
#include "../mutators/SubexpressionMutator.hpp"
#include "../mutators/TruncateMutator.hpp"
#include "../Solver.hpp"
namespace SymbolicRegression
{
using namespace std;

shared_ptr<Expression> MutatorReproducer::CreateOffspring(const shared_ptr<Expression> p1, const shared_ptr<Expression> p2)
{
    shared_ptr<Expression> child;
    // Copy parent 1
    float parentFitness = p2->Fitness();
    // Mutate child
    do
    {
        if (Expression::RandomF() > 0.5f)
        {
            child = Expression::Copy(p2);
        }
        else
        {
            child = Expression::Copy(p1);
        }

        child = SubexpressionMutator::Mutate(child);
        if (Expression::RandomF() > 0.5f)
        {
            child = TruncateMutator::Mutate(child);
        }
        else
        {
            child = ConstantMutator::Mutate(child);
        }
        child = Expression::Simplify(child);
        if (Expression::RandomF() < Solver::GetTemp())
        {
            return child;
        }
    } while (child->Fitness() < parentFitness);

    return child;
}
} // namespace SymbolicRegression

```

RandomReproducer.hpp

```cpp
#ifndef _RANDOM_REPRODUCER_HPP_
#define _RANDOM_REPRODUCER_HPP_
#include "Reproducer.hpp"
namespace SymbolicRegression
{
using namespace std;
class RandomReproducer : public Reproducer
{
public:
    inline RandomReproducer(int populationCount) : Reproducer(populationCount){};
    virtual shared_ptr<Expression> CreateOffspring(const shared_ptr<Expression> p1, const shared_ptr<Expression> p2) override;
};
} // namespace SymbolicRegression

#endif
```

RandomReproducer.cpp

```cpp
#include "RandomReproducer.hpp"
#include <algorithm>

namespace SymbolicRegression
{
using namespace std;

shared_ptr<Expression> RandomReproducer::CreateOffspring(const shared_ptr<Expression> p1, const shared_ptr<Expression> p2)
{
    return Expression::GenerateRandomExpression(0, true);
}
} // namespace SymbolicRegression

```

Reproducer.hpp

```cpp
#ifndef _REPRODUCER_HPP_
#define _REPRODUCER_HPP_
#include <memory>
#include <list>
#include <map>
#include <string>
#include <tuple>
#include "../../expression/Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class Reproducer
{
public:
    Reproducer(int populationCount);
    shared_ptr<list<shared_ptr<Expression>>> AsyncReproduce(
        const list<shared_ptr<Expression>> &parents);
    virtual shared_ptr<list<shared_ptr<Expression>>> Reproduce(
        const list<shared_ptr<Expression>> &parents);
    virtual shared_ptr<Expression> CreateOffspring(
        const shared_ptr<Expression> p1, const shared_ptr<Expression> p2) = 0;

protected:
    int m_populationCount = -1;
    map<string, shared_ptr<Expression>> m_offsprings;
    void TryInsertOffspring(shared_ptr<Expression> exp);

private:
    tuple<shared_ptr<Expression>, shared_ptr<Expression>> ChooseTwoParents(const list<shared_ptr<Expression>> &parents);

private:
    bool m_stop;
    int m_minThreads = -1;
    int m_maxThreads = -1;
};
} // namespace SymbolicRegression

#endif
```

Reproducer.cpp

```cpp
#include "Reproducer.hpp"
#include <mutex>
#include <future>
#include <deque>
#include <algorithm>
#include "../engine/Config.hpp"
namespace SymbolicRegression
{
using namespace std;
mutex mu;

Reproducer::Reproducer(int populationCount)
{
    m_populationCount = populationCount;
    m_minThreads = Config::GetInt("MinThreads");
    m_maxThreads = Config::GetInt("MaxThreads");
}

void Reproducer::TryInsertOffspring(shared_ptr<Expression> exp)
{
    if (exp->Depth() > Config::GetInt("MaxDepth"))
    {
        cout << "Invalid Expression of depth " << exp->Depth() << endl;
        return;
    }
    lock_guard<mutex> lock(mu);
    if (m_stop)
        return;
    // insert offspring if it is unique
    if (Expression::IsValid(exp) && m_offsprings.find(exp->ToString()) == m_offsprings.end())
    {
        m_offsprings.insert(make_pair(exp->ToString(), exp));
    }
    if (m_offsprings.size() >= m_populationCount)
    {
        m_stop = true;
    }
}

tuple<shared_ptr<Expression>, shared_ptr<Expression>> Reproducer::ChooseTwoParents(const list<shared_ptr<Expression>> &parents)
{
    int parentCount = int(parents.size());
    if (parentCount < 2)
        return make_tuple(parents.front(), parents.front());
    int idx1 = int(Expression::RandomF(0, 1000000)) % parentCount;
    int idx2 = int(Expression::RandomF(0, 1000000)) % parentCount;
    while (true)
    {
        idx2 = (idx2 + 1) % parentCount;
        auto p1 = parents.begin();
        advance(p1, idx1);
        auto p2 = parents.begin();
        advance(p2, idx2);
        if ((*p1)->ToString() != (*p2)->ToString())
        {
            return make_tuple(*p1, *p2);
        }
    }
}

shared_ptr<list<shared_ptr<Expression>>> Reproducer::AsyncReproduce(const list<shared_ptr<Expression>> &parents)
{
    m_offsprings.clear();
    m_stop = false;
    while (!m_stop)
    {
        size_t n = max(m_populationCount - (int)m_offsprings.size(), m_minThreads);
        if (m_maxThreads > 0)
            n = min(int(n), m_maxThreads);
        vector<future<void>> tasks;
        tasks.reserve(n);
        while (tasks.size() < tasks.capacity())
        {
            auto pair = ChooseTwoParents(parents);
            // queue up asynchronous reproduce task
            tasks.push_back(async([&]() {
                TryInsertOffspring(CreateOffspring(get<0>(pair), get<1>(pair)));
            }));
        }
        for_each(tasks.begin(), tasks.end(), [](future<void> &task) {
            task.get();
        });
    }
    shared_ptr<list<shared_ptr<Expression>>> output(new list<shared_ptr<Expression>>(m_offsprings.size()));
    transform(m_offsprings.begin(), m_offsprings.end(), output->begin(), [](auto p) {
        return p.second;
    });

    return output;
}

shared_ptr<list<shared_ptr<Expression>>> Reproducer::Reproduce(const list<shared_ptr<Expression>> &parents)
{
    m_offsprings.clear();
    int parentCount = int(parents.size());
    while (m_offsprings.size() < m_populationCount)
    {
        auto pair = ChooseTwoParents(parents);
        auto offspring = CreateOffspring(get<0>(pair), get<1>(pair));
        if (Expression::IsValid(offspring) && m_offsprings.find(offspring->ToString()) == m_offsprings.end())
        {
            m_offsprings.insert(make_pair(offspring->ToString(), offspring));
        }
    }

    shared_ptr<list<shared_ptr<Expression>>> output(new list<shared_ptr<Expression>>(m_offsprings.size()));
    transform(m_offsprings.begin(), m_offsprings.end(), output->begin(), [](auto p) {
        return p.second;
    });
    return output;
}
} // namespace SymbolicRegression

```
