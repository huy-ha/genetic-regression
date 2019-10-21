#include "AsexualReproducer.hpp"
#include "../Solver.hpp"
namespace SymbolicRegression
{
shared_ptr<list<shared_ptr<Expression>>> AsexualReproducer::Reproduce(
    const list<shared_ptr<Expression>> &parents)
{
    m_offsprings.clear();
    int parentCount = int(parents.size());
    while (m_offsprings.size() < m_populationCount)
    {
        int idx = int(Expression::RandomF(0.0f, float(m_populationCount - 1)));
        auto parent = parents.begin();
        advance(parent, idx);
        shared_ptr<Expression> offspring;
        do
        {
            offspring = Expression::Copy(*parent);
        } while (offspring->Fitness() < (*parent)->Fitness() || Expression::RandomF() < Solver::GetTemp());
        if (Expression::IsValid(offspring) && m_offsprings.find(offspring->ToString()) == m_offsprings.end())
        {
            m_offsprings.insert(make_pair(offspring->ToString(), offspring));
        }
    }
    shared_ptr<list<shared_ptr<Expression>>> output(new list<shared_ptr<Expression>>(m_offsprings.size()));
    transform(m_offsprings.begin(), m_offsprings.end(), output->begin(), [](auto p) {
        return p.second;
    });
    return output;
}
} // namespace SymbolicRegression