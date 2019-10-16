#include <iostream>
#include <time.h>
#include "engine/Config.hpp"
#include <algorithm>
#include "genetic-programming/Solver.hpp"
#include "engine/OutputLogger.hpp"
#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace SymbolicRegression;
int main(int argc, char **argv)
{
    cout << "enterring" << endl;
    {
        auto exp1 = Expression::GenerateRandomExpression(0);
        cout << exp1->ToString() << endl;
    }
    cout << "exited " << endl;
    return 0;

    int seed = int(time(NULL));
    string configFile = "default.config";
    string outputDir;
    if (argc < 2)
    {
        cout << "Please supply an output directory" << endl;
        return -1;
    }
    outputDir = string(argv[1]);
    if (argc > 2)
    {
        // output dir and config supplied
        configFile = string(argv[2]);
    }
    if (argc > 3)
    {
        seed = stoi(argv[3]);
    }
    cout << endl
         << "SEED:" << seed << endl
         << endl;
    srand(seed);
    try
    {
        Config config(configFile, outputDir);
        cout << string(config) << endl;
        OutputLogger::Log("Config", "Seed:" + to_string(seed));
        OutputLogger::Log("Config", string(config));
        auto solver = Solver::Instance();
        auto start = high_resolution_clock::now();
        solver->Run();
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