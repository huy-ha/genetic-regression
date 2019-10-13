#include <iostream>
#include <math.h>
#include "expression/Expression.hpp"
#include <memory>
#include <time.h>
#include "engine/Config.hpp"

#include "expression/Cos.hpp"
using namespace std;

int main()
{
    try
    {
        SymbolicRegression::Config config("default.config");
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    cout << "data size in main()" << SymbolicRegression::Config::Data->size() << endl;
    // srand(time(NULL));
    srand(0);
    for (int i = 0; i < 50; i++)
    {
        std::shared_ptr<SymbolicRegression::Expression> random_exp =
            SymbolicRegression::Expression::GenerateRandomExpression();
        cout << random_exp->ToString() << ":" << random_exp->Fitness() << endl;
    }
}