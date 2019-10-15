#include "SubexpressionMutator.hpp"
#include <vector>
#include <algorithm>
#include "../../engine/Config.hpp"
namespace SymbolicRegression
{
using namespace std;

shared_ptr<Expression> SubexpressionMutator::Mutate(shared_ptr<Expression> exp)
{
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
    auto expToChange = operators[int(Expression::RandomF(0, 100)) % operators.size()];

    // change a random subexpression of the operator
    expToChange->m_subexpressions[int(Expression::RandomF(0, float(expToChange->Order()) - 1.0f))] = Expression::GenerateRandomExpression(expToChange->Level() + 1);

    return exp;
}
} // namespace SymbolicRegression
