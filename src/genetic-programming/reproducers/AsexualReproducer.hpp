#ifndef _ASEXUAL_REPRODUCER_HPP_
#define _ASEXUAL_REPRODUCER_HPP_
#include "Reproducer.hpp"
namespace SymbolicRegression
{
using namespace std;
class AsexualReproducer : public Reproducer
{
public:
    inline AsexualReproducer(int populationCount) : Reproducer(populationCount){};
    virtual shared_ptr<list<shared_ptr<Expression>>> Reproduce(
        const list<shared_ptr<Expression>> &parents) override;
    inline shared_ptr<Expression> CreateOffspring(const shared_ptr<Expression> p1, const shared_ptr<Expression> p2) override
    {
        return Expression::GenerateRandomExpression(0, true);
    }
};
} // namespace SymbolicRegression

#endif