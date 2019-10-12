#ifndef _VARIABLE_HPP_
#define _VARIABLE_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Variable : public Expression
{
public:
    inline Variable() : Expression()
    {
        m_func = [&](float x) {
            return x;
        };
        m_order = 0;
    }
    inline virtual std::string ToString() const override
    {
        return "x";
    }
};
} // namespace SymbolicRegression
#endif