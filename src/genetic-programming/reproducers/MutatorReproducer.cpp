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
