#ifndef _REPRODUCER_HPP_
#define _REPRODUCER_HPP_
#include <memory>
#include <list>
#include <map>
#include "../expression/Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class Reproducer
{
public:
    inline Reproducer(int populationCount) : m_populationCount(populationCount) {}
    shared_ptr<list<shared_ptr<Expression>>> Reproduce(const list<shared_ptr<Expression>> &parents);

protected:
    int m_populationCount = -1;
    map<string, shared_ptr<Expression>> m_offsprings;
    void TryInsertOffspring(shared_ptr<Expression> exp);
    virtual shared_ptr<Expression> CreateOffspring(shared_ptr<Expression> p1, shared_ptr<Expression> p2) = 0;

private:
    bool m_stop;
};
} // namespace SymbolicRegression

#endif