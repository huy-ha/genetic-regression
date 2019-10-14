#include "RandomReproducer.hpp"
#include <algorithm>
#include <future>
#include <mutex>
#include <deque>
#include <chrono>
namespace SymbolicRegression
{
using namespace std;
mutex mu;
void RandomReproducer::TryInsertOffspring(shared_ptr<Expression> exp)
{
    lock_guard<mutex> lock(mu);
    if (offsprings.size() < m_populationCount &&
        (offsprings.find(exp->ToString()) == offsprings.end()))
        offsprings.insert(make_pair(exp->ToString(), exp));
}

shared_ptr<list<shared_ptr<Expression>>> RandomReproducer::Reproduce(const list<shared_ptr<Expression>> &parents)
{
    offsprings.clear();
    deque<future<void>> tasks;
    while (offsprings.size() < m_populationCount)
    {
        tasks.push_back(async([&]() {
            TryInsertOffspring(Expression::GenerateRandomExpression(true));
        }));
    }
    shared_ptr<list<shared_ptr<Expression>>> output(new list<shared_ptr<Expression>>(offsprings.size()));
    transform(offsprings.begin(), offsprings.end(), output->begin(), [](auto p) {
        return p.second;
    });
    return output;
} // namespace SymbolicRegression
} // namespace SymbolicRegression
