#ifndef _COS_HPP_
#define _COS_HPP_
#include "Expression.hpp"
#include <math.h>
class Cos : public Expression
{
public:
    inline Cos() : Expression(cosf) {}
    inline virtual std::string ToString() const override
    {
        return "cos";
    }
};
#endif