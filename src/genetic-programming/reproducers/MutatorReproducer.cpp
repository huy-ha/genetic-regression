#include "RandomReproducer.hpp"
#include <algorithm>
#include "MutatorReproducer.hpp"
#include <iostream>
#include "../mutators/ConstantMutator.hpp"
#include "../mutators/SubexpressionMutator.hpp"

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
        child = Expression::Copy(p2);
        child = SubexpressionMutator::Mutate(child);
        child = ConstantMutator::Mutate(child);
        child = Expression::Simplify(child);
    } while (child->Fitness() < parentFitness);

    return child;
}
} // namespace SymbolicRegression
