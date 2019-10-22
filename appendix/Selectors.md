## Selectors

DiversitySelector.hpp

```cpp
#ifndef _DIVERSITY_SELECTOR_HPP_
#define _DIVERSITY_SELECTOR_HPP_
#include "Selector.hpp"
#include "TournamentSelector.hpp"
namespace SymbolicRegression
{
class DiversitySelector : public Selector
{
public:
    inline DiversitySelector() : Selector()
    {
        m_tournamentSelector = TournamentSelector();
    }
    virtual tuple<shared_ptr<Expression>, shared_ptr<Expression>> Select(
        const list<shared_ptr<Expression>> &population) override;

private:
    TournamentSelector m_tournamentSelector;
};
} // namespace SymbolicRegression

#endif
```

DiversitySelector.cpp

```cpp

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
```

NichingSelector.hpp

```cpp
#ifndef _NICHING_SELECTOR_HPP_
#define _NICHING_SELECTOR_HPP_
#include "Selector.hpp"
#include "TournamentSelector.hpp"
namespace SymbolicRegression
{
class NichingSelector : public Selector
{
public:
    inline NichingSelector() : Selector()
    {
        m_tournamentSelector = TournamentSelector();
    }
    virtual tuple<shared_ptr<Expression>, shared_ptr<Expression>> Select(
        const list<shared_ptr<Expression>> &population) override;

protected:
    TournamentSelector m_tournamentSelector;
};
} // namespace SymbolicRegression

#endif
```

NichingSelector.cpp

```cpp
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
        cout << "\tNiching trying " << threshold << endl;
        auto [p1, p2] = m_tournamentSelector.Select(population);
        if (Expression::Diversity(p1, p2) < threshold || attempt > 50)
        {
            cout << "Niching DONE " << threshold << endl;
            return make_tuple(p1, p2);
        }
        attempt += 1;
        threshold *= growth;
    }
}
} // namespace SymbolicRegression

```

Selector.hpp

```cpp
#ifndef _SELECTOR_HPP_
#define _SELECTOR_HPP_
#include "../../expression/Expression.hpp"
#include <memory>
#include <tuple>
namespace SymbolicRegression
{
using namespace std;
class Selector
{
public:
    virtual tuple<shared_ptr<Expression>, shared_ptr<Expression>> Select(
        const list<shared_ptr<Expression>> &population) = 0;
};
} // namespace SymbolicRegression

#endif
```

TournamentSelector.hpp

```cpp
#ifndef _TOURNAMENT_SELECTOR_HPP_
#define _TOURNAMENT_SELECTOR_HPP_
#include "Selector.hpp"
namespace SymbolicRegression
{
class TournamentSelector : public Selector
{
public:
    TournamentSelector();
    virtual tuple<shared_ptr<Expression>, shared_ptr<Expression>> Select(
        const list<shared_ptr<Expression>> &population) override;

protected:
    int m_numPlayers = -1;
    shared_ptr<Expression> DoTournament(const list<shared_ptr<Expression>> &population);
};
} // namespace SymbolicRegression

#endif
```

TournamentSelector.cpp

```cpp
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
```
