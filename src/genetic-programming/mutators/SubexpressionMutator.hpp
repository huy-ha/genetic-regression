#ifndef _SUBEXPRESSION_MUTATOR_HPP_
#define _SUBEXPRESSION_MUTATOR_HPP_
#include <memory>
#include "../../expression/Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class SubexpressionMutator
{
public:
    static shared_ptr<Expression> Mutate(shared_ptr<Expression> exp);
};
} // namespace SymbolicRegression

#endif