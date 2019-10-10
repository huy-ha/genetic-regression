#include <iostream>
#include <math.h>
#include "Expression.hpp"
#include "Constant.hpp"
#include "Sin.hpp"
#include "Cos.hpp"
using namespace std;

int main()
{
    Constant k(0);
    Cos cos_exp;
    cout << k.ToString() << endl;
    cos_exp.AddSubexpression(&k);
    cout << "cos_exp(1) = " << cos_exp(1) << endl;
    cout << "cos_exp(2) = " << cos_exp(2) << endl;
    cout << "cos_exp(3) = " << cos_exp(3) << endl;
}