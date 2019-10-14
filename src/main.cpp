#include <iostream>
#include <time.h>
#include "engine/Config.hpp"
#include <algorithm>
#include "genetic-programming/Solver.hpp"
#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace SymbolicRegression;
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "Please supply an output directory" << endl;
        return -1;
    }
    try
    {
        Config config("default.config", string(argv[1]));
        cout << string(config) << endl;
        srand(0);
        Solver solver = Solver();
        auto start = high_resolution_clock::now();
        solver.Run();
        auto stop = high_resolution_clock::now();
        auto ms = duration_cast<milliseconds>(stop - start);
        float seconds = ms.count() / 1000.0f;
        cout << "Total Time of Run: " << seconds << endl;
    }
    catch (exception e)
    {
        cout << e.what() << endl;
        return -1;
    }
}