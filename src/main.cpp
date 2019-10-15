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
    // srand(time(NULL));
    srand(24);
    auto exp1 = Expression::GenerateRandomExpression();
    auto exp2 = Expression::Copy(exp1);
    auto f1 = exp1->ToFunction();
    auto f2 = exp2->ToFunction();
    cout << "started with \t" << exp1->ToString() << endl;
    cout << "f1(0) = " << f1(0) << endl;
    cout << "f1(1) = " << f1(1) << endl;
    cout << "f1(2) = " << f1(2) << endl;

    cout << "ended with \t" << exp2->ToString() << endl;
    cout << "f2(0) = " << f2(0) << endl;
    cout << "f2(1) = " << f2(1) << endl;
    cout << "f2(2) = " << f2(2) << endl;
    return 0;

    string configFile = "default.config";
    string outputDir;
    if (argc < 2)
    {
        cout << "Please supply an output directory" << endl;
        return -1;
    }
    outputDir = string(argv[1]);
    if (argc == 3)
    {
        // output dir and config supplied
        configFile = string(argv[2]);
    }
    try
    {
        Config config(configFile, outputDir);
        cout << string(config) << endl;
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