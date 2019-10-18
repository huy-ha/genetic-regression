#include "ConstantMutator.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../expression/Constant.hpp"

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
            return EXPRESSION_TYPE(e) == CONSTANT_T;
        });
    if (constants.size() == 0)
        return exp;
    i = i % constants.size();
    auto constantToMutate = dynamic_pointer_cast<Constant>(constants[i]);

    float k, bestK;
    float prevFitness = exp->Fitness();
    float testFitness = -1;
    for (int i = 0; i < 10; i++)
    {
        auto f = [=](float x) { return k; };
        testFitness = exp->Fitness(f);
        if (testFitness > prevFitness)
        {
            prevFitness = testFitness;
            bestK = k;
        }
    }

    if (prevFitness == exp->Fitness())
    {
        return exp;
    }
    constantToMutate->m_k = bestK;
    return exp;
}
} // namespace SymbolicRegression
