#ifndef _CONSTANT_HPP_
#define _CONSTANT_HPP_
#include "Expression.hpp"
#include <string>

namespace SymbolicRegression
{
class Constant : public Expression
{
public:
    Constant(); // Constructor to generate random constant
    Constant(float k);
    inline Constant(const Constant &other) : Expression(other)
    {
        m_k = other.m_k;
        m_func = [&](float x) { return m_k; };
        m_order = 0;
    };
    inline virtual std::string ToString() const override
    {
        float threedecimalplaces = float((int)(m_k * 1000 + 0.5f)) / 1000;
        return std::to_string(threedecimalplaces);
    }
    friend class ConstantMutator;

protected:
    float m_k; // value of constant
};
} // namespace SymbolicRegression
#endif
