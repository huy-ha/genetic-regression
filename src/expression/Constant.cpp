#include "Constant.hpp"
#include <iostream>
namespace SymbolicRegression
{

SymbolicRegression::Constant::Constant() : Expression()
{
    m_k = SymbolicRegression::Expression::RandomF(-10, 10);
    m_func = [&](float x) { return m_k; };
    m_order = 0;
}

SymbolicRegression::Constant::Constant(float k) : Expression()
{
    m_k = k;
    m_func = [&](float x) { return m_k; };
    m_order = 0;
}
} // namespace SymbolicRegression