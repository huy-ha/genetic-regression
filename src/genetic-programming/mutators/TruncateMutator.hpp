#ifndef _TRUNCATE_MUTATOR_HPP_
#define _TRUNCATE_MUTATOR_HPP_
#include <memory>
#include "../../expression/Expression.hpp"
namespace SymbolicRegression
{
using namespace std;
class TruncateMutator
{
public:
    static shared_ptr<Expression> Mutate(shared_ptr<Expression> exp);
};
} // namespace SymbolicRegression

#endif