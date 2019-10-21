#include "SubexpressionMutator.hpp"
#include <vector>
#include <algorithm>
#include "../../engine/Config.hpp"
#include "../Solver.hpp"
namespace SymbolicRegression
{
using namespace std;

shared_ptr<Expression> SubexpressionMutator::Mutate(shared_ptr<Expression> exp)
{
    shared_ptr<Expression> bestMutation;
    for (int i = 0; i < Config::GetInt("MutationRetries"); i++)
    {
        auto tempExp = Expression::Copy(exp);
        auto collapsedExp = exp->Collapse(exp);
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

        // choose random operator
        auto expToChange = operators[int(Expression::RandomF(0, 100)) %
                                     operators.size()];

        // change a random subexpression of the operator
        int idx = int(Expression::RandomF(0, float(expToChange->Order() - 1)));
        expToChange->m_subexpressions[idx] =
            Expression::GenerateRandomExpression(expToChange->Level() + 1);
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
