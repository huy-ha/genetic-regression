#include "TruncateMutator.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../expression/Constant.hpp"
#include "../Solver.hpp"
#include <typeinfo>
#include "../../engine/Config.hpp"
namespace SymbolicRegression
{
using namespace std;
shared_ptr<Expression> TruncateMutator::Mutate(const shared_ptr<Expression> &exp)
{
    shared_ptr<Expression> bestMutation;
    for (int i = 0; i < Config::GetInt("MutationRetries"); i++)
    {
        auto tempExp = Expression::Copy(exp);
        auto collapsedExp = tempExp->Collapse(tempExp);
        vector<shared_ptr<Expression>> operators;
        copy_if(
            collapsedExp->begin(),
            collapsedExp->end(),
            back_inserter(operators),
            [](auto e) {
                return e->Order() > 0;
            });
        if (operators.size() == 0)
            return exp;
        auto opToTruncate = operators[int(Expression::RandomF(0, 100)) %
                                      operators.size()];
        opToTruncate->m_subexpressions[int(Expression::RandomF(0, 100)) %
                                       opToTruncate->m_subexpressions.size()] =
            Expression::GenerateRandomZeroOrderExpression(opToTruncate->Level() + 1);
        if (!bestMutation || tempExp->Fitness() > bestMutation->Fitness())
        {
            bestMutation = tempExp;
        }
    }

    if (bestMutation->Fitness() > exp->Fitness() ||
        Expression::RandomF() < Solver::GetTemp())
    {
        return bestMutation;
    }
    return exp;
}
} // namespace SymbolicRegression
