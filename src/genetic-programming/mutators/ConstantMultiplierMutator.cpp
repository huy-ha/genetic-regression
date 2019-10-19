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
shared_ptr<Expression> ConstantMultiplierMutator::Mutate(shared_ptr<Expression> exp)
{
    float k, prevFitness;
    prevFitness = exp->Fitness();
    vector<shared_ptr<Expression>>::iterator it;
    // TODO try increasing or decreasing
    for (int i = 0; i < 1; i++)
    {
        // Find subexpression of type constant
        it = find_if(exp->m_subexpressions.begin(),
                     exp->m_subexpressions.end(),
                     [](auto subexp) {
                         return EXPRESSION_TYPE(subexp) == CONSTANT_T;
                     });
        // Tree is already too tall, and don't already
        // have constants multiplied in top level
        // to be mutated
        if (exp->Depth() >= Config::GetInt("MaxDepth") &&
            it == exp->m_subexpressions.end())
        {
            break;
        }

        k = Expression::RandomF(-2, 2);
        auto f = [=](float x) { return k; };
        if (exp->Fitness(f) > prevFitness)
        {
            // If expression already has multiply expression in top level
            // and constant in second level
            if (EXPRESSION_TYPE(exp) == MULTIPLY_T &&
                it != exp->m_subexpressions.end())
            {
                auto constExp = shared_ptr<Expression>(new Constant(1, k));
                replace(exp->m_subexpressions.begin(),
                        exp->m_subexpressions.end(),
                        *it, constExp);
                prevFitness = exp->Fitness();
            }
            // Create new constant and multiplier expression
            else
            {
                auto constExp = shared_ptr<Expression>(new Constant(1, k));
                exp = shared_ptr<Expression>(new Multiply(0, constExp, exp));
                prevFitness = exp->Fitness();
            }
        }
    }

    return exp;
}
} // namespace SymbolicRegression
