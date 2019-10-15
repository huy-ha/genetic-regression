#ifndef _CONSTANT_HPP_
#define _CONSTANT_HPP_
#include "Expression.hpp"
#include <string>
#include <iomanip>
#include <sstream>
namespace SymbolicRegression
{
class Constant : public Expression
{
public:
    inline Constant(shared_ptr<Expression> parent) : Expression(parent)
    {
        m_k = SymbolicRegression::Expression::RandomF(-10, 10);
        m_func = [&](float x) { return m_k; };
        m_order = 0;
    }
    inline Constant(shared_ptr<Expression> parent, float k) : Expression(parent)
    {
        m_k = k;
        m_func = [&](float x) { return m_k; };
        m_order = 0;
    }
    inline Constant(const Constant &other) : Expression(other)
    {
        m_k = other.m_k;
        m_func = [&](float x) { return m_k; };
        m_order = 0;
    };
    inline virtual std::string ToString() const override
    {
        stringstream s;
        s << fixed << setprecision(3) << m_k;
        return s.str();
    }
    friend class ConstantMutator;

protected:
    float m_k; // value of constant
};
} // namespace SymbolicRegression
#endif
