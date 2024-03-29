#include "ConstantMutator.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../expression/Constant.hpp"
#include "../Solver.hpp"
#include "../../engine/Config.hpp"

namespace SymbolicRegression
{
using namespace std;
shared_ptr<Expression> ConstantMutator::Mutate(const shared_ptr<Expression> &exp)
{
    auto tempExp = Expression::Copy(exp);
    auto collapsedExp = tempExp->Collapse(tempExp);
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
    float prevFitness = tempExp->Fitness();
    float testFitness = -1;
    for (int i = 0; i < Config::GetInt("MutationRetries"); i++)
    {
        auto f = [=](float x) { return k; };
        testFitness = tempExp->Fitness(f);
        if (testFitness > prevFitness || Expression::RandomF() < Solver::GetTemp())
        {
            prevFitness = testFitness;
            bestK = k;
        }
    }

    if (prevFitness > exp->Fitness() || Expression::RandomF() < Solver::GetTemp())
    {
        constantToMutate->m_k = bestK;
        return tempExp;
    }
    return exp;
}
} // namespace SymbolicRegression
