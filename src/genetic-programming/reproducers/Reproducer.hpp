#ifndef _REPRODUCER_HPP_
#define _REPRODUCER_HPP_
#include <memory>
#include <list>
#include <map>
#include <string>
#include <tuple>
#include "../../expression/Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class Reproducer
{
public:
    Reproducer(int populationCount);
    shared_ptr<list<shared_ptr<Expression>>> AsyncReproduce(
        const list<shared_ptr<Expression>> &parents);
    virtual shared_ptr<list<shared_ptr<Expression>>> Reproduce(
        const list<shared_ptr<Expression>> &parents);
    virtual shared_ptr<Expression> CreateOffspring(
        const shared_ptr<Expression> p1, const shared_ptr<Expression> p2) = 0;

protected:
    int m_populationCount = -1;
    map<string, shared_ptr<Expression>> m_offsprings;
    void TryInsertOffspring(shared_ptr<Expression> exp);

private:
    tuple<shared_ptr<Expression>, shared_ptr<Expression>> ChooseTwoParents(const list<shared_ptr<Expression>> &parents);

private:
    bool m_stop;
    int m_minThreads = -1;
    int m_maxThreads = -1;
};
} // namespace SymbolicRegression

#endif