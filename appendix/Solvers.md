## Solver

Solver.hpp

```cpp
#ifndef _SOLVER_HPP_
#define _SOLVER_HPP_
#include <list>
#include <memory>
#include <mutex>
#include "reproducers/Reproducer.hpp"
#include "selectors/Selector.hpp"
#include "../engine/OutputLogger.hpp"
#include <ctime>
#include <chrono>
namespace SymbolicRegression
{
using namespace std;
class Solver
{
public:
    Solver();
    virtual void Run() = 0;
    void SaveOutput();
    static shared_ptr<Solver> Instance();
    static float GetTemp();
    static void DecayTemp();
    void SavePopulationFitnesses();
    float PopulationDiversity();
    inline bool ShouldStop()
    {
        return m_prevBestEvaluations < OutputLogger::GetEvaluations() / 2;
    }

protected:
    void InitializePopulation();
    void PrintPopulation();

protected:
    list<shared_ptr<Expression>> m_population;
    int m_populationCount = -1;
    shared_ptr<Reproducer> m_reproducer;
    int m_eliteCount = -1;
    float m_prevHighestFitness = -1;
    int m_prevBestEvaluations = -1;
    shared_ptr<Expression> m_prevBest;
    shared_ptr<Selector> m_selector;

private:
    static shared_ptr<Solver> m_instance;
    static float m_temperature;
    static mutex tempMutex;
    bool collectDataForDotPlot;
};
} // namespace SymbolicRegression
#endif
```

Solver.cpp

```cpp
#include "Solver.hpp"
#include "../expression/Expression.hpp"
#include <functional>
#include <algorithm>
#include "../engine/Config.hpp"
#include "../engine/OutputLogger.hpp"
#include "reproducers/RandomReproducer.hpp"
#include "reproducers/MutatorReproducer.hpp"
#include "reproducers/AsexualReproducer.hpp"
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
    else if (reproducerConfig == "Asexual")
    {
        m_reproducer = shared_ptr<Reproducer>(new AsexualReproducer(m_populationCount));
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
    OutputLogger::Log("FinalBest", m_prevBest->ToString());
    auto f = m_prevBest->ToFunction();
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

```

GenerationalSolver.hpp

```cpp
#ifndef _GENERATIONAL_SOLVER_HPP_
#define _GENERATIONAL_SOLVER_HPP_
#include "Solver.hpp"
namespace SymbolicRegression
{
class GenerationalSolver : public Solver
{
public:
    virtual void Run() override;

protected:
    void Evolve();
};
} // namespace SymbolicRegression

#endif
```

GenerationalSolver.cpp

```cpp
#include "GenerationalSolver.hpp"
#include "../engine/Config.hpp"
#include <algorithm>
#include <iterator>
#include "../engine/OutputLogger.hpp"
namespace SymbolicRegression
{

void GenerationalSolver::Run()
{
    int generationCount = Config::GetInt("GenerationCount");
    int saveEval = 0;
    InitializePopulation();
    for (int i = 0; i < generationCount; i++)
    {
        Evolve();
        DecayTemp();
        if (OutputLogger::GetEvaluations() > saveEval * 100000)
        {
            SaveOutput();
            saveEval++;
        }
    }
    SaveOutput();
    m_population.sort(Expression::FitnessComparer);
    auto finalBest = *m_population.begin();
    cout << "FITNESS " << m_prevHighestFitness
         << " after " << OutputLogger::GetEvaluations() << " evalutions at temp " << GetTemp() << endl;
    cout << "\t" + finalBest->ToString() << endl;
}

void GenerationalSolver::Evolve()
{
    for_each(m_population.begin(), m_population.end(),
             [](const shared_ptr<Expression> &exp) { exp->Fitness(); });
    // Sort in decreasing order of fitness
    m_population.sort(Expression::FitnessComparer);
    // Find best in current population
    shared_ptr<Expression> bestExpression = *(m_population.begin());
    if (bestExpression->Fitness() > m_prevHighestFitness)
    {
        m_prevHighestFitness = bestExpression->Fitness();
        cout << "FITNESS " << m_prevHighestFitness
             << " after " << OutputLogger::GetEvaluations() << " evalutions at temp " << GetTemp() << endl;
        cout << "\t" + bestExpression->ToString() << endl;
    }
    // Selection
    auto it = m_population.begin();
    advance(it, m_population.size() * 0.7f);
    m_population.erase(it, m_population.end());
    // Reproduce
    // auto offspring = m_reproducer->AsyncReproduce(m_population);
    auto offspring = m_reproducer->Reproduce(m_population);

    // Handle Elites
    auto eliteEnd = m_population.begin();
    advance(eliteEnd, m_eliteCount);
    list<shared_ptr<Expression>> elites(m_population.begin(), eliteEnd);
    // Create new population
    m_population.clear();
    copy(elites.begin(), elites.end(), front_inserter(m_population));
    copy(offspring->begin(), offspring->end(), front_inserter(m_population));
    m_population.sort(Expression::FitnessComparer);

    // Ensure size of population
    if (m_population.size() < m_populationCount)
    {
        cout << "ERROR: LOST EXPRESSIONS" << endl;
        throw exception("Lost Expressions");
    }
    else if (m_population.size() > m_populationCount)
    {
        m_population.resize(m_populationCount);
    }
    // Collect Stats
    OutputLogger::Log("HighestFitness", to_string(OutputLogger::GetEvaluations()) + " " + to_string(m_prevHighestFitness));
}
}
```

ContinuousSolver.hpp

```cpp
#ifndef _CONTINUOUS_SOLVER_HPP_
#define _CONTINUOUS_SOLVER_HPP_
#include "Solver.hpp"
namespace SymbolicRegression
{
class ContinuousSolver : public Solver
{
public:
    virtual void Run() override;

protected:
    void EvolveRound();
};
} // namespace SymbolicRegression

#endif
```

ContinuousSolver.cpp

```cpp
#include "ContinuousSolver.hpp"
#include "../engine/Config.hpp"
#include "../engine/OutputLogger.hpp"
#include <chrono>
#include <thread>
namespace SymbolicRegression
{
using namespace std;

void ContinuousSolver::EvolveRound()
{
    if (typeid(*m_reproducer).name() == string("class SymbolicRegression::RandomReproducer") ||
        typeid(*m_reproducer).name() == string("class SymbolicRegression::AsexualReproducer"))
    {
        auto offsprings = m_reproducer->Reproduce(m_population);
        m_population.clear();
        for_each(offsprings->begin(), offsprings->end(), [&](auto e) {
            m_population.emplace_front(e);
        });
    }
    else
    {
        // select two parents using selector
        auto [p1, p2] = m_selector->Select(m_population);
        // remove parents from population
        // reproduce the two parents, giving at least 2 individuals back as a list
        auto offspring = m_reproducer->CreateOffspring(p1, p2);
        bool notunique = any_of(m_population.begin(), m_population.end(), [&](auto p) {
            return p->ToString() == offspring->ToString();
        });
        if (!Expression::IsValid(offspring) || notunique)
        {
            return;
        }
        m_population.emplace_front(offspring);
        // remove all invalid expressions
        // handle over population
        m_population.sort(Expression::FitnessComparer);
        while (m_population.size() > m_populationCount)
        {
            m_population.pop_back();
        }
    }
    OutputLogger::Log("HighestFitness",
                      to_string(OutputLogger::GetEvaluations()) +
                          "," + to_string(m_prevHighestFitness));
}

void ContinuousSolver::Run()
{
    m_prevHighestFitness = -1;
    int saveEval = 0;
    InitializePopulation();
    int round = 0;
    // Run for at least 6000 rounds
    // Afterwards, if double evaluations without getting better, quit
    while (round < 6000 || !ShouldStop())
    {
        EvolveRound();
        for_each(m_population.begin(), m_population.end(), [](auto e) {
            e = Expression::Simplify(e);
        });
        m_population.sort(Expression::FitnessComparer);
        if ((*m_population.begin())->Fitness() > m_prevHighestFitness)
        {
            m_prevBest = (*m_population.begin());
            m_prevHighestFitness = m_prevBest->Fitness();
            m_prevBestEvaluations = OutputLogger::GetEvaluations();
            cout << "FITNESS: " << m_prevHighestFitness << " | Temp " << GetTemp() << endl;
            cout << "\t" << m_prevBest->ToString() << endl;
            SaveOutput();
        }
        else if (OutputLogger::GetEvaluations() > saveEval * 100)
        {
            SaveOutput();
            saveEval++;
        }
        DecayTemp();
        SavePopulationFitnesses();
        OutputLogger::Log("Diversity",
                          to_string(OutputLogger::GetEvaluations()) +
                              "," + to_string(PopulationDiversity()));
        round += 1;
    }
    SaveOutput();
    cout << "FITNESS " << m_prevHighestFitness
         << " after " << OutputLogger::GetEvaluations() << " evalutions at temp " << GetTemp() << endl;
    cout << "\t" + m_prevBest->ToString() << endl;
}
} // namespace SymbolicRegression
```
