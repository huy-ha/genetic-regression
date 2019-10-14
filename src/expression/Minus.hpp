#ifndef _MINUS_HPP_
#define _MINUS_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Minus : public Expression
{
public:
    Minus();
    inline Minus(const Minus &other) : Expression(other)
    {
        m_func = [&](float x) {
            return m_subexpressions[0]->ToFunction()(x) - m_subexpressions[1]->ToFunction()(x);
        };
        m_order = 2;
    };
    std::string ToString() const override;
};
} // namespace SymbolicRegression
#endif