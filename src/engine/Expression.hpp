#ifndef _EXPRESSION_HPP_
#define _EXPRESSION_HPP_
#include <functional>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
namespace SymbolicRegression
{
class Expression
{
public:
    std::function<float(float)> ToFunction();
    float Evaluate(float x);
    float operator()(float x);
    void AddSubexpression(Expression *subexpression);
    virtual std::string ToString() const;
    std::shared_ptr<Expression> static GenerateRandomExpression();
    static float RandomF();
    static float RandomF(float min, float max);

protected:
    Expression();
    Expression(std::function<float(float)> f);
    template <typename R, typename... Types>
    inline static int NumArgs(std::function<R(Types...)> f) { return sizeof...(Types); }

protected:
    int m_order = -1;                                 // How many parameters the current expression needs
    std::function<float(float)> m_func = 0;           //function presenting this expression node's function
    std::function<float(float)> m_expressionFunc = 0; // function representing entire expression
    std::vector<Expression *> m_subexpressions;
    enum ExpressionType
    {
        Sin,
        Cos,
        Plus,
        Minus,
        Multiply,
        Divide,
        Constant,
        Variable
    };
};
} // namespace SymbolicRegression
// std::ostream &operator<<(std::ostream &os, const Expression &e)
// {
//     os << e.ToString();
//     return os;
// }
#endif
