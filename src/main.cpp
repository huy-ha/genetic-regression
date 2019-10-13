#include <iostream>
#include <time.h>
#include "engine/Config.hpp"
#include <algorithm>
#include "genetic-programming/Solver.hpp"
using namespace std;
using namespace SymbolicRegression;
int main()
{
    try
    {
        Config config("default.config");
    }
    catch (exception e)
    {
        cout << e.what() << endl;
        return -1;
    }
    // srand(time(NULL));
    cout << "done setting up config" << endl;
    srand(0);
    Solver solver = Solver();
    solver.Evolve();
}