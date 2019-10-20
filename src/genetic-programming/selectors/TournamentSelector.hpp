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