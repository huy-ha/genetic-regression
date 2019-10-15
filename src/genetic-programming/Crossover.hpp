#ifndef _CROSSOVER_HPP_
#define _CROSSOVER_HPP_
#include <memory>
#include "../expression/Expression.hpp"
namespace SymbolicRegression
{
class Crossover
{
public:
    static shared_ptr<Expression> Cross(const shared_ptr<Expression> &p1, const shared_ptr<Expression> &p2);
};
} // namespace SymbolicRegression

#endif