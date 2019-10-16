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
public:
    // Factory Functions
    shared_ptr<Expression> static GenerateRandomExpression(int level, bool noConstant = false, bool noZero = false, bool noTrig = false);
    shared_ptr<Expression> static GenerateRandomZeroOrderExpression(int level);
    shared_ptr<Expression> static GenerateRandomBinaryOperator(int level);

    friend class SubexpressionMutator;
    friend class CrossoverMutatorReproducer;

public:
    /*
        STATIC HELPER FUNCTIONS
    */
    static function<bool(const shared_ptr<Expression> &, const shared_ptr<Expression> &)> FitnessComparer;

    static shared_ptr<Expression> Initialize(shared_ptr<Expression> self, shared_ptr<Expression> parent);

    shared_ptr<vector<shared_ptr<Expression>>> Collapse(shared_ptr<Expression> self);
    static shared_ptr<Expression> Simplify(shared_ptr<Expression> exp);

    static float RandomF();
    static float RandomF(float min, float max);
    virtual string ToString() const = 0;
    static shared_ptr<Expression> Copy(const shared_ptr<Expression> &source);

public:
    /*
        PUBLIC ACCESSORS
    */
    virtual function<float(float)> ToFunction() const;

    float Fitness();
    float CalculateFitness() const;

    inline int Order() const { return m_order; }
    inline int Level() const { return m_level; }
    int Depth() const;
    static bool IsValid(shared_ptr<Expression> exp);

protected:
    Expression(int level);
    Expression(const Expression &other);
    template <typename R, typename... Types>
    inline static int NumArgs(function<R(Types...)> f) { return sizeof...(Types); }

protected:
    int m_order = -1;                  // How many parameters the current expression needs
    function<float(float)> m_func = 0; //function presenting this expression node's function
    shared_ptr<Expression> m_parent;
    vector<shared_ptr<Expression>> m_subexpressions;
    float m_fitness = -1;
    int m_level = -1;

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
