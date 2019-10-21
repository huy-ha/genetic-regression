#include "ConstantMultiplierMutator.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../expression/Constant.hpp"
#include "../../expression/Multiply.hpp"
#include "../../engine/Config.hpp"

namespace SymbolicRegression
{
using namespace std;

// TRY OUT SAME EXPRESSION WITH A DIFFERENT CONSTANT MULTIPLIER
shared_ptr<Expression> ConstantMultiplierMutator::Mutate(const shared_ptr<Expression> &exp)
{
    float k, prevFitness;
    auto tempExp = Expression::Copy(exp);
    prevFitness = tempExp->Fitness();
    vector<shared_ptr<Expression>>::iterator it;
    for (int i = 0; i < Config::GetInt("MutationRetries"); i++)
    {
        // Find subexpression of type constant
        it = find_if(tempExp->m_subexpressions.begin(),
                     tempExp->m_subexpressions.end(),
                     [](auto subexp) {
                         return EXPRESSION_TYPE(subexp) == CONSTANT_T;
                     });
        // Tree is already too tall, and don't already
        // have constants multiplied in top level
        // to be mutated
        if (tempExp->Depth() >= Config::GetInt("MaxDepth") &&
            it == tempExp->m_subexpressions.end())
        {
            break;
        }

        k = Expression::RandomF(-2, 2);
        auto f = [=](float x) { return k; };
        if (tempExp->Fitness(f) > prevFitness)
        {
            // If expression already has multiply expression in top level
            // and constant in second level
            if (EXPRESSION_TYPE(tempExp) == MULTIPLY_T &&
                it != tempExp->m_subexpressions.end())
            {
                auto constExp = shared_ptr<Expression>(new Constant(1, k));
                replace(tempExp->m_subexpressions.begin(),
                        tempExp->m_subexpressions.end(),
                        *it, constExp);
                prevFitness = tempExp->Fitness();
            }
            // Create new constant and multiplier expression
            else
            {
                auto constExp = shared_ptr<Expression>(new Constant(1, k));
                tempExp = shared_ptr<Expression>(new Multiply(0, constExp, tempExp));
                prevFitness = tempExp->Fitness();
            }
        }
    }

    return exp;
}
} // namespace SymbolicRegression
