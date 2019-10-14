#ifndef _RANDOm_REPRODUCER_HPP_
#define _RANDOm_REPRODUCER_HPP_
#include "Reproducer.hpp"

namespace SymbolicRegression
{
using namespace std;
class RandomReproducer : public Reproducer
{
public:
    inline RandomReproducer(int populationCount) : Reproducer(populationCount) {}

protected:
    virtual shared_ptr<Expression> CreateOffspring(shared_ptr<Expression> p1, shared_ptr<Expression> p2) override;
};
} // namespace SymbolicRegression

#endif