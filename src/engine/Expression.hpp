#ifndef _EXPRESSION_HPP_
#define _EXPRESSION_HPP_
#include <functional>
#include <vector>
class Expression
{
public:
    Expression(std::function<float(float)> f) : m_func(f) {}
    std::function<float(float)> ToFunction();
    float Evaluate(float x);

protected:
    std::function<float(float)> m_func = 0;           //function presenting this expression node's function
    std::function<float(float)> m_expressionFunc = 0; // function representing entire expression
    std::vector<Expression> *m_subexpressions = 0;
};
#endif