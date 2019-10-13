#include "RandomReproducer.hpp"

namespace SymbolicRegression
{
shared_ptr<list<shared_ptr<Expression>>> RandomReproducer::Reproduce(const list<shared_ptr<Expression>> &parents)
{
    shared_ptr<list<shared_ptr<Expression>>> offsprings(new list<shared_ptr<Expression>>());
    for (int i = 0; i < m_populationCount; ++i)
    {
        offsprings->emplace_front(Expression::GenerateRandomExpression());
    }
    return offsprings;
}
} // namespace SymbolicRegression
