#ifndef _EXPRESSION_HPP_
#define _EXPRESSION_HPP_
#include <functional>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <algorithm>
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
    shared_ptr<Expression> static GenerateRandomTrigExpression(int level);
    shared_ptr<Expression> static GenerateRandomBinaryOperator(int level);

    friend class SubexpressionMutator;
    friend class TruncateMutator;
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
    inline virtual string ToString() const = 0;
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
    inline static vector<shared_ptr<Expression>>::iterator FindFirst(
        vector<shared_ptr<Expression>> &operators,
        function<bool(const shared_ptr<Expression> &)> predicate)
    {
        return find_if(operators.begin(), operators.end(), [&](auto op) {
            return all_of(op->m_subexpressions.begin(), op->m_subexpressions.end(), predicate);
        });
    }
    inline void RecalculateLevels(int level)
    {
        m_level = level;
        for_each(m_subexpressions.begin(), m_subexpressions.end(), [&](auto subexp) {
            subexp->RecalculateLevels(level + 1);
        });
    }
    // static shared_ptr<Expression> SimplifyOperator(
    //     vector<shared_ptr<Expression>> &operators,
    //     function<bool(const shared_ptr<Expression> &)> predicate,
    //     function<shared_ptr<Expression>(int)> replacementConstructor);

protected:
    int m_order = -1;                  // How many parameters the current expression needs
    function<float(float)> m_func = 0; //function presenting this expression node's function
    weak_ptr<Expression> m_parent;
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
