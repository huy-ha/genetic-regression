#include "AsexualReproducer.hpp"
#include "../Solver.hpp"
#include "../mutators/ConstantMultiplierMutator.hpp"
#include "../mutators/ConstantMutator.hpp"
#include "../mutators/SubexpressionMutator.hpp"
#include "../mutators/TrigMultiplierMutator.hpp"
#include "../mutators/TruncateMutator.hpp"
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
        int attempts = 0;
        do
        {
            offspring = Expression::Copy(*parent);
            offspring = ConstantMutator::Mutate(offspring);
            if (Expression::RandomF() > 0.7f)
            {
                //cout  << "cons mult" << endl;
                offspring = ConstantMultiplierMutator::Mutate(offspring);
            }
            if (Expression::RandomF() > 0.7f)
            {
                //cout  << "trig mult" << endl;
                offspring = TrigMultiplierMutator::Mutate(offspring);
            }
            if (Expression::RandomF() > 0.7f)
            {
                //cout  << "subexp" << endl;
                offspring = SubexpressionMutator::Mutate(offspring);
            }
            if (Expression::RandomF() > 0.7f)
            {
                //cout  << "truncate" << endl;
                offspring = TruncateMutator::Mutate(offspring);
            }
            attempts += 1;
        } while (offspring->Fitness() < (*parent)->Fitness() ||
                 Expression::RandomF() < Solver::GetTemp() ||
                 attempts > 1000000);
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