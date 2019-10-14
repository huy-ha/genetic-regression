#include "Solver.hpp"
#include "../engine/Config.hpp"
#include "../expression/Expression.hpp"
#include "../engine/Config.hpp"
#include <functional>
#include <algorithm>
#include <iterator>
#include "../engine/OutputLogger.hpp"
#include "RandomReproducer.hpp"
#include <fstream>
#include <windows.h>

namespace SymbolicRegression
{
using namespace std;
Solver::Solver()
{
    m_populationCount = Config::GetInt("PopulationCount");
    m_eliteCount = Config::GetInt("ElitesCount");
    string reproducerConfig = Config::GetString("Reproducer");
    // if (reproducerConfig == "Random")
    // {
    m_reproducer = shared_ptr<Reproducer>(new RandomReproducer(m_populationCount));
    // }
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
        auto newExp = Expression::GenerateRandomExpression(true);
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
    InitializePopulation();
    for (int i = 0; i < generationCount; i++)
    {
        Evolve();
    }
    SaveOutput();
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
             << " after " << OutputLogger::Evaluations << " evaluations" << endl;
    }
    // Selection
    auto it = m_population.begin();
    advance(it, m_population.size() * 0.5f);
    m_population.erase(it, m_population.end());

    // Reproduce
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
    OutputLogger::Log("HighestFitness", to_string(OutputLogger::Evaluations) + " " + to_string(m_prevHighestFitness));
}

void Solver::SaveOutput()
{
    string dirpath = Config::GetString("OutputPath");
    LPCSTR w_dirpath = LPCSTR(dirpath.c_str());
    //  wstring(dirpath.begin(), dirpath.end()).c_str();
    if (CreateDirectory(w_dirpath, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
    {
        // Directory created
        ofstream highestFitnessOutput;
        highestFitnessOutput.open(dirpath + "HighestFitness.txt");
        if (highestFitnessOutput.is_open())
        {
            highestFitnessOutput << OutputLogger::Get("HighestFitness");
        }
        else
        {
            cout << "Error opening " << dirpath + "HighestFitness.txt" << endl;
        }
        highestFitnessOutput.close();
    }
    else
    {
        // Failed for some other reason
        cout << "Failed to create output directory" << endl;
    }

} // namespace SymbolicRegression
} // namespace SymbolicRegression
