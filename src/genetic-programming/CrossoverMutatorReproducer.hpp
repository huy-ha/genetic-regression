#ifndef _CROSSOVER_MUTATOR_REPRODUCER_HPP_
#define _CROSSOVER_MUTATOR_REPRODUCER_HPP_
#include "Reproducer.hpp"
class CrossoverMutatorReproducer : public Reproducer
{
public:
    inline RandomReproducer(int populationCount) : Reproducer(populationCount){};
    shared_ptr<list<shared_ptr<Expression>>> Reproduce(const list<shared_ptr<Expression>> &parents) override;
}
#endif