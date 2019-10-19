#ifndef _GENERATIONAL_SOLVER_HPP_
#define _GENERATIONAL_SOLVER_HPP_
#include "Solver.hpp"
namespace SymbolicRegression
{
class GenerationalSolver : public Solver
{
public:
    virtual void Run() override;

protected:
    void Evolve();
};
} // namespace SymbolicRegression

#endif