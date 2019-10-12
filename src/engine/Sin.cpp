#include "Sin.hpp"
#include <math.h>
namespace SymbolicRegression
{
Sin::Sin() : Expression()
{
    m_subexpressions.push_back(Expression::GenerateRandomExpression());
    m_func = [&](float x) {
        return sinf(m_subexpressions[0]->ToFunction()(x));
    };
}
}