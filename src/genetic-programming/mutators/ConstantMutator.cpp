#include "ConstantMutator.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../expression/Constant.hpp"
#include <typeinfo>
namespace SymbolicRegression
{
using namespace std;
shared_ptr<Expression> ConstantMutator::Mutate(shared_ptr<Expression> exp)
{
    auto collapsedExp = exp->Collapse(exp);
    vector<shared_ptr<Expression>> constants;
    int i = int(Expression::RandomF(0, 100));
    copy_if(
        collapsedExp->begin(),
        collapsedExp->end(),
        back_inserter(constants),
        [](auto e) {
            return string(typeid(*e).name()) == string("class SymbolicRegression::Constant");
        });
    cout << constants.size() << endl;
    i = i % constants.size();
    auto constantToMutate = dynamic_pointer_cast<Constant>(constants[i]);
    constantToMutate->m_k = constantToMutate->m_k + Expression::RandomF() * 0.1f;
    return exp;
}
} // namespace SymbolicRegression
