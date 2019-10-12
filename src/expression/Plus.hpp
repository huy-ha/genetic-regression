#ifndef _PLUS_HPP_
#define _PLUS_HPP_
#include "Expression.hpp"
namespace SymbolicRegression
{
class Plus : public Expression
{
public:
    Plus();
    virtual std::string ToString() const override;
};
} // namespace SymbolicRegression
#endif