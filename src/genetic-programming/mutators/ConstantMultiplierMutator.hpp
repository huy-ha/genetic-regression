#ifndef _MULTIPLIER_MUTATOR_HPP_
#define _MULTIPLIER_MUTATOR_HPP_
#include <memory>
#include "../../expression/Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class ConstantMultiplierMutator
{
public:
    static shared_ptr<Expression> Mutate(shared_ptr<Expression> exp);
};
} // namespace SymbolicRegression

#endif