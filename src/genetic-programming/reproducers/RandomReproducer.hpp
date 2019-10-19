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