#include <iostream>
#include <time.h>
#include "engine/Config.hpp"
#include <algorithm>
#include "genetic-programming/Solver.hpp"
#include "engine/OutputLogger.hpp"
#include <chrono>

#include "expression/Sin.hpp"
#include "expression/Cos.hpp"
#include "expression/Variable.hpp"

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
        Config config(configFile + ".config", outputDir);
        // {
        //     auto varExp = shared_ptr<Expression>(new Variable(1));
        //     auto e1 = shared_ptr<Expression>(new Sin(0, varExp));
        //     auto e2 = shared_ptr<Expression>(new Sin(0, varExp));
        //     cout << e1->ToString() << " + " << varExp->ToString() << endl;
        //     cout << Expression::Diversity(e1, varExp) << endl;
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