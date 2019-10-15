#include "Solver.hpp"
#include "../expression/Expression.hpp"
#include "../engine/Config.hpp"
#include <functional>
#include <algorithm>
#include <iterator>
#include "../engine/OutputLogger.hpp"
#include "reproducers/RandomReproducer.hpp"
#include "reproducers/MutatorReproducer.hpp"
#include <fstream>
#include <windows.h>
#include "../expression/Constant.hpp"

namespace SymbolicRegression
{
using namespace std;
Solver::Solver()
{
    m_populationCount = Config::GetInt("PopulationCount");
    m_eliteCount = Config::GetInt("ElitesCount");
    string reproducerConfig = Config::GetString("Reproducer");
    if (reproducerConfig == "Mutator")
    {
        m_reproducer = shared_ptr<Reproducer>(new MutatorReproducer(m_populationCount));
    }
    else
    {
        m_reproducer = shared_ptr<Reproducer>(new RandomReproducer(m_populationCount));
    }
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
    // m_population.emplace_front(shared_ptr<Expression>(new Constant(0)));
    while (m_population.size() < m_populationCount)
    {
        auto newExp = Expression::GenerateRandomExpression(nullptr, true);
        if (!any_of(m_population.begin(), m_population.end(), [&](const shared_ptr<Expression> &exp) {
                return exp->ToString() == newExp->ToString();
            }))
        {
            m_population.emplace_front(newExp);
        }
    }
}

void Solver::Run()
{
    int generationCount = Config::GetInt("GenerationCount");
    int saveEvals = 0;
    InitializePopulation();
    for (int i = 0; i < generationCount; i++)
    {
        Evolve();
        if (OutputLogger::GetEvaluations() > saveEvals * 100000)
        {
            saveEvals++;
            cout << "saving at " << OutputLogger::GetEvaluations() << " evaluations " << endl;
            SaveOutput();
        }
    }
    SaveOutput();
    m_population.sort(Expression::FitnessComparer);
    auto finalBest = *m_population.begin();
    cout << finalBest->ToString() << endl;
}

void Solver::Evolve()
{

    for_each(m_population.begin(), m_population.end(),
             [](auto &exp) { exp->Fitness(); });
    // Sort in decreasing order of fitness
    m_population.sort();

    // Find best in current population
    shared_ptr<Expression> bestExpression = *(m_population.begin());
    if (bestExpression->Fitness() > m_prevHighestFitness)
    {
        m_prevHighestFitness = bestExpression->Fitness();
        cout << bestExpression->ToString() << " : " << m_prevHighestFitness
             << " after " << OutputLogger::GetEvaluations() << " evaluations" << endl;
    }
    // Selection
    auto it = m_population.begin();
    advance(it, m_population.size() * 0.5f);
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
    m_population.unique();

    // Ensure size of population
    if (m_population.size() < m_populationCount)
    {
        throw exception("Lost Expressions");
    }
    else if (m_population.size() > m_populationCount)
    {
        m_population.resize(m_populationCount);
    }
    // Collect Stats
    OutputLogger::Log("HighestFitness", to_string(OutputLogger::GetEvaluations()) + " " + to_string(m_prevHighestFitness));
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
    }

} // namespace SymbolicRegression
} // namespace SymbolicRegression
