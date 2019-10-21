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
        // int idx1 = int(Expression::RandomF(0, float(population.size() - 1)));
        // int idx2 = int(Expression::RandomF(0, float(population.size() - 1)));
        // while (idx1 == idx2)
        // {
        //     idx2 = int(Expression::RandomF(0, float(population.size() - 1)));
        // }
        // cout << "going with " << idx1 << " and " << idx2 << endl;
        // auto it1 = population.begin();
        // advance(it1, idx1);
        // auto it2 = population.begin();
        // advance(it2, idx2);

        // auto p1 = *it1;
        // auto p2 = *it2;
        if (Expression::Diversity(p1, p2) > threshold || attempt > 50)
        {
            return make_tuple(p1, p2);
        }
        attempt += 1;
        threshold *= decay;
    }
}
} // namespace SymbolicRegression
