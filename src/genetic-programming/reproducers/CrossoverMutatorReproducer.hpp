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