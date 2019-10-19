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
    int seed = int(time(NULL));
    string configFile = "";
    string outputDir = "tmp";
    if (argc > 1)
    {
        outputDir = string(argv[1]);
    }
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
        // {
        //     auto exp = Expression::GenerateRandomExpression(0);
        //     cout << exp->ToString() << ":" << exp->Fitness() << endl;
        //     exp = TrigMultiplierMutator::Mutate(exp);
        //     cout << exp->ToString() << ":" << exp->Fitness() << endl;
        // }
        // return 0;
        OutputLogger::Log("Config", "Seed:" + to_string(seed));
        OutputLogger::Log("Config", string(config));
        cout << string(config) << endl;
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