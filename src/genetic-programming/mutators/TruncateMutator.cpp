#include "TruncateMutator.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../expression/Constant.hpp"
#include <typeinfo>
namespace SymbolicRegression
{
using namespace std;
shared_ptr<Expression> TruncateMutator::Mutate(shared_ptr<Expression> exp)
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
    auto opToTruncate = operators[int(Expression::RandomF(0, 100)) % operators.size()];
    opToTruncate->m_subexpressions[int(Expression::RandomF(0, 100)) % opToTruncate->m_subexpressions.size()] =
        Expression::GenerateRandomZeroOrderExpression(opToTruncate->Level() + 1);
    return exp;
}
} // namespace SymbolicRegression
