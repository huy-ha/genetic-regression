#ifndef _CONSTANT_MUTATOR_HPP_
#define _CONSTANT_MUTATOR_HPP_
#include <memory>
#include "../../expression/Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class ConstantMutator
{
public:
    static shared_ptr<Expression> Mutate(const shared_ptr<Expression> &exp);
};
} // namespace SymbolicRegression

#endif