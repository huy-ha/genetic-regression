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