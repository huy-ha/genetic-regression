#include <iostream>
#include <math.h>
#include "Expression.hpp"
using namespace std;

int main()
{
    auto *exp = new Expression(sinf);
    auto func = exp->ToFunction();
    cout << func(1) << endl;
    cout << func(2) << endl;
    cout << func(3) << endl;
}
