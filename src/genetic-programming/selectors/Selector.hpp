#ifndef _SELECTOR_HPP_
#define _SELECTOR_HPP_
#include "../../expression/Expression.hpp"
#include <memory>
#include <tuple>
namespace SymbolicRegression
{
using namespace std;
class Selector
{
public:
    virtual tuple<shared_ptr<Expression>, shared_ptr<Expression>> Select(
        const list<shared_ptr<Expression>> &population) = 0;
};
} // namespace SymbolicRegression

#endif