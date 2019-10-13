#include "RandomReproducer.hpp"
#include <algorithm>
namespace SymbolicRegression
{
using namespace std;
shared_ptr<list<shared_ptr<Expression>>> RandomReproducer::Reproduce(const list<shared_ptr<Expression>> &parents)
{
    shared_ptr<list<shared_ptr<Expression>>> offsprings(new list<shared_ptr<Expression>>());
    while (offsprings->size() < m_populationCount)
    {
        auto offspring = Expression::GenerateRandomExpression();
        if (!any_of(offsprings->begin(), offsprings->end(), [&](const shared_ptr<Expression> &exp) {
                return exp->ToString() == offspring->ToString();
            }))
        {
            offsprings->emplace_front(offspring);
        }
    }
    return offsprings;
}
} // namespace SymbolicRegression
