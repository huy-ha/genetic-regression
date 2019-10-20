#include "DiversitySelector.hpp"
#include "../../expression/Expression.hpp"
namespace SymbolicRegression
{
tuple<shared_ptr<Expression>, shared_ptr<Expression>> DiversitySelector::Select(
    const list<shared_ptr<Expression>> &population)
{
    float threshold = 15.0f;
    float decay = 0.95f;
    int attempt = 0;
    while (true)
    {
        auto [p1, p2] = m_tournamentSelector.Select(population);
        if (Expression::Diversity(p1, p2) > threshold || attempt > 50)
        {
            return make_tuple(p1, p2);
        }
        attempt += 1;
        threshold *= decay;
    }
}
} // namespace SymbolicRegression
