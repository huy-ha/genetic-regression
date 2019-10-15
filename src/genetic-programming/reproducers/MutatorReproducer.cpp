#include "RandomReproducer.hpp"
#include <algorithm>
#include "MutatorReproducer.hpp"
namespace SymbolicRegression
{
using namespace std;

shared_ptr<Expression> MutatorReproducer::CreateOffspring(const shared_ptr<Expression> p1, const shared_ptr<Expression> p2)
{
    // Copy parent 1
    auto exp = Expression::Copy(p1);
    // Mutate Parent 1

    return exp;
}
} // namespace SymbolicRegression
