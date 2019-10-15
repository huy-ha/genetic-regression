#include "RandomReproducer.hpp"
#include <algorithm>
#include "MutatorReproducer.hpp"
#include <iostream>
#include "../mutators/ConstantMutator.hpp"
namespace SymbolicRegression
{
using namespace std;

shared_ptr<Expression> MutatorReproducer::CreateOffspring(const shared_ptr<Expression> p1, const shared_ptr<Expression> p2)
{
    // Copy parent 1
    auto child = Expression::Copy(p1);
    // Mutate child
    if (Expression::RandomF() > 0.9f) // 10% change float mutation
    {
        child = ConstantMutator::Mutate(child);
    }

    return child;
}
} // namespace SymbolicRegression
