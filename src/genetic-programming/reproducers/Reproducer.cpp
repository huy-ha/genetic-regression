#include "Reproducer.hpp"
#include <mutex>
#include <future>
#include <deque>
#include <algorithm>
#include "../engine/Config.hpp"
namespace SymbolicRegression
{
using namespace std;
mutex mu;

Reproducer::Reproducer(int populationCount)
{
    m_populationCount = populationCount;
    m_minThreads = Config::GetInt("MinThreads");
    m_maxThreads = Config::GetInt("MaxThreads");
}

void Reproducer::TryInsertOffspring(shared_ptr<Expression> exp)
{
    if (exp->Depth() > Config::GetInt("MaxDepth"))
    {
        cout << "Invalid Expression of depth " << exp->Depth() << endl;
        return;
    }
    lock_guard<mutex> lock(mu);
    if (m_stop)
        return;
    // insert offspring if it is unique
    if (Expression::IsValid(exp) && m_offsprings.find(exp->ToString()) == m_offsprings.end())
    {
        m_offsprings.insert(make_pair(exp->ToString(), exp));
    }
    if (m_offsprings.size() >= m_populationCount)
    {
        m_stop = true;
    }
}

tuple<shared_ptr<Expression>, shared_ptr<Expression>> Reproducer::ChooseTwoParents(const list<shared_ptr<Expression>> &parents)
{
    int parentCount = int(parents.size());
    if (parentCount < 2)
        return make_tuple(parents.front(), parents.front());
    int idx1 = int(Expression::RandomF(0, 1000000)) % parentCount;
    int idx2 = int(Expression::RandomF(0, 1000000)) % parentCount;
    while (true)
    {
        idx2 = (idx2 + 1) % parentCount;
        auto p1 = parents.begin();
        advance(p1, idx1);
        auto p2 = parents.begin();
        advance(p2, idx2);
        if ((*p1)->ToString() != (*p2)->ToString())
        {
            return make_tuple(*p1, *p2);
        }
    }
}

shared_ptr<list<shared_ptr<Expression>>> Reproducer::AsyncReproduce(const list<shared_ptr<Expression>> &parents)
{
    m_offsprings.clear();
    m_stop = false;
    while (!m_stop)
    {
        size_t n = max(m_populationCount - (int)m_offsprings.size(), m_minThreads);
        if (m_maxThreads > 0)
            n = min(int(n), m_maxThreads);
        vector<future<void>> tasks;
        tasks.reserve(n);
        while (tasks.size() < tasks.capacity())
        {
            auto pair = ChooseTwoParents(parents);
            // queue up asynchronous reproduce task
            tasks.push_back(async([&]() {
                TryInsertOffspring(CreateOffspring(get<0>(pair), get<1>(pair)));
            }));
        }
        for_each(tasks.begin(), tasks.end(), [](future<void> &task) {
            task.get();
        });
    }
    shared_ptr<list<shared_ptr<Expression>>> output(new list<shared_ptr<Expression>>(m_offsprings.size()));
    transform(m_offsprings.begin(), m_offsprings.end(), output->begin(), [](auto p) {
        return p.second;
    });

    return output;
}

shared_ptr<list<shared_ptr<Expression>>> Reproducer::Reproduce(const list<shared_ptr<Expression>> &parents)
{
    cout << "Reproducing .. " << endl;
    m_offsprings.clear();
    int parentCount = int(parents.size());
    while (m_offsprings.size() < m_populationCount)
    {
        auto pair = ChooseTwoParents(parents);
        auto offspring = CreateOffspring(get<0>(pair), get<1>(pair));
        if (m_offsprings.find(offspring->ToString()) == m_offsprings.end())
        {
            cout << "Taking " << offspring->ToString() << endl;
            m_offsprings.insert(make_pair(offspring->ToString(), offspring));
        }
        else
        {
            cout << "Rejecting " << offspring->ToString() << endl;
        }
    }

    shared_ptr<list<shared_ptr<Expression>>> output(new list<shared_ptr<Expression>>(m_offsprings.size()));
    transform(m_offsprings.begin(), m_offsprings.end(), output->begin(), [](auto p) {
        return p.second;
    });
    cout << "Done Reproducing" << endl;
    return output;
}
} // namespace SymbolicRegression
