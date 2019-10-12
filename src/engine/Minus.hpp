#ifndef _MINUS_HPP_
#define _MINUS_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Minus : public Expression
{
public:
    Minus();
    std::string ToString() const override;
};
} // namespace SymbolicRegression
#endif