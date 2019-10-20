#include "TournamentSelector.hpp"
#include "../../expression/Expression.hpp"
#include "../../engine/Config.hpp"
namespace SymbolicRegression
{

TournamentSelector::TournamentSelector() : Selector()
{
    m_numPlayers = Config::GetInt("TournamentPlayersCount");
}

shared_ptr<Expression> TournamentSelector::DoTournament(const list<shared_ptr<Expression>> &population)
{
    int populationCount = int(population.size());
    vector<int> playerIndices;
    vector<shared_ptr<Expression>> players;
    Expression::Random(0, populationCount - 1, m_numPlayers, playerIndices);
    int i = 0;
    for_each(population.begin(), population.end(), [&i, &playerIndices, &players](auto e) {
        if (find_if(
                playerIndices.begin(),
                playerIndices.end(),
                [i](int j) {
                    return j == i;
                }) != playerIndices.end())
        {
            players.push_back(e);
        }
        i += 1;
    });
    sort(players.begin(), players.end(), Expression::FitnessComparer);
    return players[0];
}

tuple<shared_ptr<Expression>, shared_ptr<Expression>> TournamentSelector::Select(
    const list<shared_ptr<Expression>> &population)
{
    shared_ptr<Expression> p2, p1 = DoTournament(population);
    do
    {
        p2 = DoTournament(population);
    } while (p2->ToString() == p1->ToString());
    return make_tuple(p1, p2);
}
} // namespace SymbolicRegression