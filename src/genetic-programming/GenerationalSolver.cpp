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