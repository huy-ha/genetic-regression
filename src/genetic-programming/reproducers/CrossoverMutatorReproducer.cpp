#include "CrossoverMutatorReproducer.hpp"
#include <memory>
#include "../../expression/Expression.hpp"
#include "../mutators/ConstantMutator.hpp"
#include "../mutators/SubexpressionMutator.hpp"
#include "../mutators/ConstantMultiplierMutator.hpp"
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
            return e->Order() > 0 && e->Depth() < (maxDepth - 1);
        });
    if (operators.size() == 0)
        return nullptr;
    auto expToChange = operators[int(Expression::RandomF(0, 100)) % operators.size()];
    return &(expToChange->m_subexpressions[int(Expression::RandomF(0, float(expToChange->Order()) - 1.0f))]);
}

shared_ptr<Expression> CrossoverMutatorReproducer::CreateOffspring(const shared_ptr<Expression> p1, const shared_ptr<Expression> p2)
{
    shared_ptr<Expression> child;
    do
    {
        // cout << "creating child from " << p1->ToString() << " and " << p2->ToString() << endl;
        child = Expression::Copy(p1);
        // Not tall enough to crossover
        if (child->Depth() == 1 || p2->Depth() == 1)
        {
            // cout << "\tDone:" << child->ToString() << endl;
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
        child = ConstantMultiplierMutator::Mutate(child);
        if (Expression::RandomF() > 0.8f)
        {
            child = SubexpressionMutator::Mutate(child);
        }
        // if (Expression::RandomF() > 0.999f)
        if (Expression::RandomF() > 0.9f)
        {
            child = TruncateMutator::Mutate(child);
        }
        if (Expression::RandomF() < Solver::GetTemp())
        {
            // cout << "\tDone:" << child->ToString() << endl;
            return child;
        }
        // child needs to be better than one of the parents
    } while (child->Fitness() < p1->Fitness() &&
             child->Fitness() < p2->Fitness());
    // cout << "\tDone:" << child->ToString() << endl;
    return child;
}

} // namespace SymbolicRegression