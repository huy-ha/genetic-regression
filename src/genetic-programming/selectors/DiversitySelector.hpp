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