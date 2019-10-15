#ifndef _EXPRESSION_HPP_
#define _EXPRESSION_HPP_
#include <functional>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <mutex>
namespace SymbolicRegression
{
using namespace std;
class Expression
{
    //TODO handle copy constructor with parent
public:
    // Main API for other classes to use
    shared_ptr<Expression> static GenerateRandomExpression(bool noConstant = false, bool noZero = false, bool noTrig = false);
    static function<bool(const shared_ptr<Expression> &, const shared_ptr<Expression> &)> FitnessComparer;
    // Function Object functionality
    virtual function<float(float)> ToFunction() const;
    float Evaluate(float x);
    float operator()(float x);
    bool operator<(const Expression &e);

    // Expression Tree
    void AddSubexpression(shared_ptr<Expression> subexpression);

    // Genetic Programming functionality
    float Fitness();
    float CalculateFitness() const;

    // Tools
    static float RandomF();
    static float RandomF(float min, float max);
    virtual string ToString() const = 0;
    static shared_ptr<Expression> Copy(const shared_ptr<Expression> &source);

    inline int Order() const { return m_order; }

    shared_ptr<vector<shared_ptr<Expression>>> Collapse(shared_ptr<Expression> self);
    friend class SubexpressionMutator;

    static shared_ptr<Expression> Initialize(shared_ptr<Expression> self, shared_ptr<Expression> parent);
    static shared_ptr<Expression> Simplify(shared_ptr<Expression> exp);

protected:
    Expression();
    Expression(const Expression &other);
    template <typename R, typename... Types>
    inline static int NumArgs(function<R(Types...)> f) { return sizeof...(Types); }

protected:
    int m_order = -1;                  // How many parameters the current expression needs
    function<float(float)> m_func = 0; //function presenting this expression node's function
    shared_ptr<Expression> m_parent = nullptr;
    vector<shared_ptr<Expression>> m_subexpressions;
    float m_fitness = -1;

private:
    static mutex randMutex;
};
} // namespace SymbolicRegression
// ostream &operator<<(ostream &os, const Expression &e)
// {
//     os << e.ToString();
//     return os;
// }
#endif
