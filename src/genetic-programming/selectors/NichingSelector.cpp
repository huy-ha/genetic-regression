#include "NichingSelector.hpp"
#include "../../expression/Expression.hpp"
namespace SymbolicRegression
{
tuple<shared_ptr<Expression>, shared_ptr<Expression>> NichingSelector::Select(
    const list<shared_ptr<Expression>> &population)
{
    float threshold = 2.0f;
    float growth = 1.05f;
    int attempt = 0;
    while (true)
    {
        auto [p1, p2] = m_tournamentSelector.Select(population);
        if (Expression::Diversity(p1, p2) < threshold || attempt > 50)
        {
            return make_tuple(p1, p2);
        }
        attempt += 1;
        threshold *= growth;
    }
}
} // namespace SymbolicRegression
