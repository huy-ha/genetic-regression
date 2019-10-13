#include "Solver.hpp"
#include "../engine/Config.hpp"
#include "../expression/Expression.hpp"
#include "../engine/Config.hpp"
#include <functional>
#include <algorithm>
#include "../engine/OutputLogger.hpp"
#include "RandomReproducer.hpp"
namespace SymbolicRegression
{
using namespace std;
Solver::Solver()
{
    cout << "trying to get populationCount" << endl;
    m_populationCount = Config::Instance->GetInt("PopulationCount");
    cout << "trying to get eliteCount" << endl;
    m_eliteCount = Config::Instance->GetInt("ElitesCount");
    m_reproducer = shared_ptr<Reproducer>(new RandomReproducer(m_populationCount));
}

void Solver::InitializePopulation()
{
    for (int i = 0; i < m_populationCount; ++i)
    {
        m_population.emplace_front(Expression::GenerateRandomExpression());
    }
    cout << "Generated population of " << m_population.size() << " expressions";
}
void Solver::Evolve()
{
    int generationCount = Config::Instance->GetInt("GenerationCount");
    float prevHighestFitness = -10000;
    InitializePopulation();
    for (int i = 0; i < generationCount; i++)
    {
        for_each(m_population.begin(), m_population.end(),
                 [](auto &exp) { exp->Fitness(); });
        // Sort in decreasing order of fitness
        m_population.sort();

        // Find best in current population
        shared_ptr<Expression> bestExpression = *(m_population.begin());
        if (bestExpression->Fitness() > prevHighestFitness)
        {
            prevHighestFitness = bestExpression->Fitness();
            cout << bestExpression->ToString() << " : " << prevHighestFitness
                 << " after" << OutputLogger::Evaluations << " evaluations" << endl;
        }
        // Selection
        auto it = m_population.begin();
        advance(it, m_population.size() * 0.5f);
        m_population.erase(it, m_population.end());
        cout << "population size is now " << m_population.size() << endl;

        // Reproduce
        auto offspring = m_reproducer->Reproduce(m_population);

        // Handle Elites
        auto eliteEnd = m_population.begin();
        advance(eliteEnd, m_eliteCount);
        list<shared_ptr<Expression>> elites(m_population.begin(), eliteEnd);

        // Create new population
        m_population.clear();
        m_population.merge(elites);
        m_population.merge(*offspring);
        m_population.sort();
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
        cout << "Done generation " << i << endl;
    }
}

void Solver::SaveOutput()
{
}
} // namespace SymbolicRegression
