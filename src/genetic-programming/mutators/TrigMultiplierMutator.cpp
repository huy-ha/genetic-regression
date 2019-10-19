#include "TrigMultiplierMutator.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../expression/Constant.hpp"
#include "../../expression/Variable.hpp"
#include "../../expression/Multiply.hpp"
#include "../../expression/Plus.hpp"
#include "../../expression/Sin.hpp"
#include "../../engine/Config.hpp"

namespace SymbolicRegression
{
using namespace std;
shared_ptr<Expression> TrigMultiplierMutator::Mutate(shared_ptr<Expression> exp)
{
    float frequency, displacement, output_frequency, output_displacement;
    float prevFitness, testFitness;
    prevFitness = exp->Fitness();
    for (int i = 0; i < 1; i++)
    {
        if (exp->Depth() >= Config::GetInt("MaxDepth"))
        {
            break;
        }
        frequency = Expression::RandomF(-10, 10);
        displacement = Expression::RandomF(-10, 10);
        auto f = [=](float x) {
            return sinf(frequency * x + displacement);
        };
        testFitness = exp->Fitness(f);
        if (testFitness > prevFitness)
        {
            prevFitness = testFitness;
            output_frequency = frequency;
            output_displacement = displacement;
        }
    }

    if (prevFitness == exp->Fitness())
    {
        return exp;
    }
    auto frequencyConstant = shared_ptr<Expression>(new Constant(-1, output_frequency));
    auto displacementConstant = shared_ptr<Expression>(new Constant(-1, output_displacement));
    auto variable = shared_ptr<Expression>(new Variable(-1));

    auto variableTimesFrequency = shared_ptr<Expression>(new Multiply(0, frequencyConstant, variable));
    auto signalTerm = shared_ptr<Expression>(new Plus(0, variableTimesFrequency, displacementConstant));

    auto sinExpression = shared_ptr<Expression>(new Sin(0, signalTerm));
    exp = shared_ptr<Expression>(new Multiply(0, sinExpression, exp));
    return exp;
}
} // namespace SymbolicRegression
