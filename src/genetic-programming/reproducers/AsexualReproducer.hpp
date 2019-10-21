#ifndef _ASEXUAL_REPRODUCER_HPP_
#define _ASEXUAL_REPRODUCER_HPP_
#include "Reproducer.hpp"
namespace SymbolicRegression
{
using namespace std;
class AsexualReproducer : public Reproducer
{
public:
    virtual shared_ptr<list<shared_ptr<Expression>>> Reproduce(
        const list<shared_ptr<Expression>> &parents) override;
};
} // namespace SymbolicRegression

#endif