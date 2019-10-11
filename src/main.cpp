#include <iostream>
#include <math.h>
#include "Expression.hpp"
#include "Constant.hpp"
#include "Sin.hpp"
#include "Cos.hpp"
#include <memory>
#include <time.h>

using namespace std;

int main()
{
    srand(time(NULL));

    for (int i = 0; i < 40; i++)
    {
        std::shared_ptr<SymbolicRegression::Expression> random_exp =
            SymbolicRegression::Expression::GenerateRandomExpression();
        cout << random_exp->ToString() << endl;
    }
}