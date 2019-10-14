#include "RandomReproducer.hpp"
#include <algorithm>

namespace SymbolicRegression
{
using namespace std;

shared_ptr<Expression> RandomReproducer::CreateOffspring(shared_ptr<Expression> p1, shared_ptr<Expression> p2)
{
    return Expression::GenerateRandomExpression(true);
}
} // namespace SymbolicRegression
