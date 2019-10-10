#include "Constant.hpp"

Constant::Constant(float k) : Expression()
{
    m_k = k;
    m_func = [&](float x) { return m_k; };
    m_expressionFunc = m_func;
    m_order = 0;
}