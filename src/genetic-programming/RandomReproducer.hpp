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
    shared_ptr<list<shared_ptr<Expression>>> Reproduce(const list<shared_ptr<Expression>> &parents) override;
};
} // namespace SymbolicRegression

#endif