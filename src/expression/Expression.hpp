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
    // Main API for other classes to use
    std::shared_ptr<Expression> static GenerateRandomExpression();

    // Function Object functionality
    virtual std::function<float(float)> ToFunction();
    float Evaluate(float x);
    float operator()(float x);

    // Expression Tree
    void AddSubexpression(std::shared_ptr<Expression> subexpression);

    // Genetic Programming functionality
    float Fitness();

    // Tools
    static float RandomF();
    static float RandomF(float min, float max);
    virtual std::string ToString() const = 0;

protected:
    Expression();
    template <typename R, typename... Types>
    inline static int NumArgs(std::function<R(Types...)> f) { return sizeof...(Types); }

protected:
    int m_order = -1;                       // How many parameters the current expression needs
    std::function<float(float)> m_func = 0; //function presenting this expression node's function
    std::vector<std::shared_ptr<Expression>> m_subexpressions;
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

private:
};
} // namespace SymbolicRegression
// std::ostream &operator<<(std::ostream &os, const Expression &e)
// {
//     os << e.ToString();
//     return os;
// }
#endif
