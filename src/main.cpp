#include <iostream>
#include <time.h>
#include "engine/Config.hpp"
#include <algorithm>
#include "genetic-programming/Solver.hpp"
#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace SymbolicRegression;
int main()
{
    try
    {
        Config config("default.config");
        cout << string(config) << endl;
        srand(0);
        Solver solver = Solver();
        auto start = high_resolution_clock::now();
        solver.Evolve();
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<seconds>(stop - start);
        cout << duration.count() << endl;
    }
    catch (exception e)
    {
        cout << e.what() << endl;
        return -1;
    }
}