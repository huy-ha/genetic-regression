#include "Reproducer.hpp"
#include <mutex>
#include <future>
#include <deque>
#include <algorithm>
namespace SymbolicRegression
{
using namespace std;
mutex mu;
void Reproducer::TryInsertOffspring(shared_ptr<Expression> exp)
{
    lock_guard<mutex> lock(mu);
    if (m_stop)
        return;
    // insert offspring if it is unique
    if (m_offsprings.find(exp->ToString()) == m_offsprings.end())
    {
        m_offsprings.insert(make_pair(exp->ToString(), exp));
    }
    if (m_offsprings.size() >= m_populationCount)
    {
        m_stop = true;
    }
}

shared_ptr<list<shared_ptr<Expression>>> Reproducer::Reproduce(const list<shared_ptr<Expression>> &parents)
{
    m_offsprings.clear();
    m_stop = false;
    int parentCount = int(parents.size());
    while (!m_stop)
    {
        size_t n = max(m_populationCount - (int)m_offsprings.size(), 20);
        cout << "trying to create " << n << " offsprings" << endl;
        vector<future<void>> tasks;
        tasks.reserve(n);
        while (tasks.size() < tasks.capacity())
        {
            // find two random parents
            int idx1 = int(Expression::RandomF(0, float(parentCount)));
            int idx2 = (idx1 + (int(Expression::RandomF(0, float(parentCount))))) % parentCount;
            auto p1 = parents.begin();
            advance(p1, idx1);
            auto p2 = parents.begin();
            advance(p2, idx2);
            // queue up asynchronous reproduce task
            tasks.push_back(async([&]() { TryInsertOffspring(CreateOffspring(*p1, *p2)); }));
        }
        for_each(tasks.begin(), tasks.end(), [](const future<void> &task) {
            task.wait();
        });
        cout << "done creating" << endl;
    }
    shared_ptr<list<shared_ptr<Expression>>> output(new list<shared_ptr<Expression>>(m_offsprings.size()));
    transform(m_offsprings.begin(), m_offsprings.end(), output->begin(), [](auto p) {
        return p.second;
    });

    return output;
}
} // namespace SymbolicRegression
