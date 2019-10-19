#ifndef _CONTINUOUS_SOLVER_HPP_
#define _CONTINUOUS_SOLVER_HPP_
#include "Solver.hpp"
namespace SymbolicRegression
{
class ContinuousSolver : public Solver
{
public:
    virtual void Run() override;

protected:
    void EvolveRound();
};
} // namespace SymbolicRegression

#endif