#ifndef _REPRODUCER_HPP_
#define _REPRODUCER_HPP_
#include <memory>
#include <list>
#include "../expression/Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class Reproducer
{
public:
    inline Reproducer(int populationCount) : m_populationCount(populationCount) {}
    virtual shared_ptr<list<shared_ptr<Expression>>> Reproduce(const list<shared_ptr<Expression>> &parents) = 0;

protected:
    int m_populationCount = -1;
};
} // namespace SymbolicRegression

#endif