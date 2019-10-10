#ifndef _CONSTANT_HPP_
#define _CONSTANT_HPP_
#include "Expression.hpp"
#include <string>
class Constant : public Expression
{
public:
    Constant(float k);
    inline virtual std::string ToString() const override
    {
        return std::to_string(m_k);
    }

protected:
    float m_k; // value of constant
};
#endif