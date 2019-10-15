#include "RandomReproducer.hpp"
#include <algorithm>

namespace SymbolicRegression
{
using namespace std;

shared_ptr<Expression> RandomReproducer::CreateOffspring(const shared_ptr<Expression> p1, const shared_ptr<Expression> p2)
{
    return Expression::GenerateRandomExpression(nullptr, true);
}
} // namespace SymbolicRegression
