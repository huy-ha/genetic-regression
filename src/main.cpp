#include <iostream>
#include <math.h>
#include "Expression.hpp"
#include <memory>
#include <time.h>
#include "engine/Plus.hpp"
using namespace std;

int main()
{
    srand(time(NULL));
    for (int i = 0; i < 50; i++)
    {
        std::shared_ptr<SymbolicRegression::Expression> random_exp =
            SymbolicRegression::Expression::GenerateRandomExpression();
        cout << random_exp->ToString() << endl;
    }
}