#ifndef _VARIABLE_HPP_
#define _VARIABLE_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Variable : public Expression
{
public:
    inline Variable(int level) : Expression(level)
    {
        m_func = [&](float x) {
            return x;
        };
        m_order = 0;
        m_subexpressions.clear();
    }

    inline Variable(const Variable &other) : Expression(other)
    {
        m_func = [&](float x) {
            return x;
        };
        m_order = 0;
        m_subexpressions.clear();
    }

    inline virtual std::string ToString() const override
    {
        return "x";
    }
};
} // namespace SymbolicRegression
#endif