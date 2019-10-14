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

protected:
    virtual shared_ptr<Expression> CreateOffspring(const shared_ptr<Expression> p1, const shared_ptr<Expression> p2) override;
};
} // namespace SymbolicRegression

#endif