#ifndef _SIN_HPP_
#define _SIN_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Sin : public Expression
{
public:
    inline Sin() : Expression()
    {
        m_subexpressions.push_back(Expression::GenerateRandomExpression(true, false, true));
        m_func = [&](float x) {
            return sinf(m_subexpressions[0]->ToFunction()(x));
        };
        m_order = 1;
    }

    inline Sin(const Sin &other) : Expression(other)
    {
        m_func = [&](float x) {
            return sinf(m_subexpressions[0]->ToFunction()(x));
        };
        m_order = 1;
    };

    inline virtual std::string ToString() const override
    {
        return "sin(" + m_subexpressions[0]->ToString() + ")";
    }
};
} // namespace SymbolicRegression
#endif