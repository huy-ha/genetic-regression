#include "SubexpressionMutator.hpp"
#include <vector>
#include <algorithm>
namespace SymbolicRegression
{
using namespace std;
shared_ptr<Expression> SubexpressionMutator::Mutate(shared_ptr<Expression> exp)
{
    auto collapsedExp = exp->Collapse(exp);
    vector<shared_ptr<Expression>> operators;
    int i = int(Expression::RandomF(0, 100));
    copy_if(
        collapsedExp->begin(),
        collapsedExp->end(),
        back_inserter(operators),
        [](auto e) {
            return e->Order() > 0;
        });
    if (operators.size() == 0)
        return exp;
    i = i % operators.size();
    auto expToChange = operators[i];
    i = int(Expression::RandomF(0, float(expToChange->Order()) - 1.0f));
    expToChange->m_subexpressions[i] = Expression::GenerateRandomExpression(expToChange);
    return exp;
}
} // namespace SymbolicRegression
