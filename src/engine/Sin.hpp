#ifndef _SIN_HPP_
#define _SIN_HPP_
#include "Expression.hpp"
#include <math.h>
class Sin : public Expression
{
public:
    inline Sin() : Expression(sinf) {}
    inline virtual std::string ToString() const override
    {
        return "sin";
    }
};
#endif