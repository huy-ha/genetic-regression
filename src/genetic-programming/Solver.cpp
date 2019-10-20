#include "Solver.hpp"
#include "../expression/Expression.hpp"
#include <functional>
#include <algorithm>
#include "../engine/Config.hpp"
#include "../engine/OutputLogger.hpp"
#include "reproducers/RandomReproducer.hpp"
#include "reproducers/MutatorReproducer.hpp"
#include "reproducers/CrossoverMutatorReproducer.hpp"
#include "selectors/TournamentSelector.hpp"
#include "selectors/DiversitySelector.hpp"
#include "selectors/NichingSelector.hpp"
#include <fstream>
#include <windows.h>
#include "../expression/Constant.hpp"
#include <cmath>
#include "GenerationalSolver.hpp"
#include "ContinuousSolver.hpp"
namespace SymbolicRegression
{
using namespace std;
shared_ptr<Solver> Solver::m_instance;
float Solver::m_temperature;
mutex Solver::tempMutex;
Solver::Solver()
{
    m_populationCount = Config::GetInt("PopulationCount");
    m_eliteCount = Config::GetInt("ElitesCount");
    m_temperature = Config::GetFloat("Init_T");
    string reproducerConfig = Config::GetString("Reproducer");
    if (reproducerConfig == "CrossoverMutator")
    {
        m_reproducer = shared_ptr<Reproducer>(new CrossoverMutatorReproducer(m_populationCount));
    }
    else if (reproducerConfig == "Mutator")
    {
        m_reproducer = shared_ptr<Reproducer>(new MutatorReproducer(m_populationCount));
    }
    else
    {
        m_reproducer = shared_ptr<Reproducer>(new RandomReproducer(m_populationCount));
    }
    string selectorConfig = Config::GetString("Selector");
    if (selectorConfig == "Diversity")
    {
        m_selector = shared_ptr<Selector>(new DiversitySelector());
    }
    else if (selectorConfig == "Niching")
    {
        m_selector = shared_ptr<Selector>(new NichingSelector());
    }
    else
    {
        m_selector = shared_ptr<Selector>(new TournamentSelector());
    }

    collectDataForDotPlot = Config::GetInt("DotPlot");
}

shared_ptr<Solver> Solver::Instance()
{
    if (!m_instance)
    {
        string solverConfig = Config::GetString("Solver");
        if (solverConfig == "Continuous")
        {
            m_instance = shared_ptr<Solver>(new ContinuousSolver());
        }
        else
        {
            m_instance = shared_ptr<Solver>(new GenerationalSolver());
        }
    }
    return m_instance;
}

void Solver::PrintPopulation()
{
    for_each(m_population.begin(), m_population.end(), [](auto exp) {
        cout << exp->ToString() << " | ";
    });
    cout << endl;
}
void Solver::InitializePopulation()
{
    while (m_population.size() < m_populationCount)
    {
        auto newExp = Expression::GenerateRandomExpression(0, true);
        if (Expression::IsValid(newExp) && !any_of(m_population.begin(), m_population.end(), [&](const shared_ptr<Expression> &exp) {
                return exp->ToString() == newExp->ToString();
            }))
        {
            m_population.emplace_front(newExp);
        }
    }
}

float Solver::PopulationDiversity()
{
    int n = int(m_population.size());
    float diversitySum = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            auto e1 = m_population.begin();
            advance(e1, i);
            auto e2 = m_population.begin();
            advance(e2, j);
            diversitySum += Expression::Diversity(*e1, *e2);
        }
    }
    return diversitySum * 0.0001f / n * n;
}

void Solver::SaveOutput()
{
    OutputLogger::Clear("FinalBest");
    m_population.sort(Expression::FitnessComparer);
    auto finalBest = *m_population.begin();
    OutputLogger::Log("FinalBest", finalBest->ToString());
    auto f = finalBest->ToFunction();
    for (float x = 0; x < 10; x += 0.001f)
    {
        OutputLogger::Log("FinalBest", to_string(x) + " " + to_string(f(x)));
    }
    string dirpath = Config::GetString("OutputPath");
    LPCSTR w_dirpath = LPCSTR(dirpath.c_str());
    //  wstring(dirpath.begin(), dirpath.end()).c_str();
    if (CreateDirectory(w_dirpath, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
    {
        // Directory created
        auto keys = OutputLogger::GetKeys();
        for (auto &key : *keys)
        {
            ofstream f;
            f.open(dirpath + key + ".txt");
            if (f.is_open())
            {
                f << OutputLogger::Get(key);
            }
            else
            {
                cout << "Error opening " << dirpath + key + ".txt" << endl;
            }
            f.close();
        }
    }
    else
    {
        // Failed for some other reason
        cout << "Failed to create output directory" << endl;
        exit(-1);
    }
}

void Solver::SavePopulationFitnesses()
{
    string evals = to_string(OutputLogger::GetEvaluations());
    for_each(m_population.begin(), m_population.end(), [&evals](auto e) {
        OutputLogger::Log("FitnessDotPlot", evals + "," + to_string(e->Fitness()));
    });
}

float Solver::GetTemp()
{
    lock_guard<mutex> lock(tempMutex);
    return m_temperature;
}

void Solver::DecayTemp()
{
    lock_guard<mutex> lock(tempMutex);
    m_temperature *= Config::GetFloat("T_decay");
}
} // namespace SymbolicRegression
