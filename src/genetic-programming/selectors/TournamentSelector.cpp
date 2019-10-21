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
    vector<shared_ptr<Expression>> players;
    if (m_numPlayers == population.size())
    {
        return *max_element(population.begin(), population.end(), Expression::FitnessComparer);
    }
    else if (m_numPlayers > population.size())
    {
        cout << "ERROR:PARENTS LESS THAN TOURNAMNET SIZE" << endl;
        throw exception();
    }
    while (players.size() < m_numPlayers)
    {
        auto it = population.begin();
        int idx = int(Expression::RandomF(0.0f, float(populationCount - 1)));
        advance(it, idx);

        bool notunique = any_of(players.begin(), players.end(), [&](auto p) {
            return p->ToString() == (*it)->ToString();
        });
        if (!notunique)
        {
            players.push_back(*it);
        }
    }
    sort(players.begin(), players.end(), Expression::FitnessComparer);
    return players[0];
}

tuple<shared_ptr<Expression>, shared_ptr<Expression>> TournamentSelector::Select(
    const list<shared_ptr<Expression>> &population)
{
    if (population.size() == 2)
    {
        return make_tuple(population.front(), population.back());
    }
    else if (population.size() < 2)
    {
        cout << "ERROR: POPULATION TOO SMALL" << endl;
        throw exception();
    }
    list<shared_ptr<Expression>> parents = list<shared_ptr<Expression>>(population.begin(), population.end());
    shared_ptr<Expression> p2,
        p1 = DoTournament(parents);
    parents.remove(p1);
    do
    {
        p2 = DoTournament(parents);
        parents.remove(p2);
    } while (p2->ToString() == p1->ToString());
    return make_tuple(p1, p2);
}
} // namespace SymbolicRegression