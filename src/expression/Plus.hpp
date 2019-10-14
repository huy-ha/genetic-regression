#ifndef _PLUS_HPP_
#define _PLUS_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Plus : public Expression
{
public:
    Plus();
    inline Plus(const Plus &other) : Expression(other)
    {
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) + m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    };
    virtual std::string ToString() const override;
};
} // namespace SymbolicRegression
#endif