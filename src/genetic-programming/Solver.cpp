#include "Solver.hpp"
#include "../engine/Config.hpp"
#include "../expression/Expression.hpp"
#include "../engine/Config.hpp"
#include <functional>
#include <algorithm>
#include <iterator>
#include "../engine/OutputLogger.hpp"
#include "RandomReproducer.hpp"
namespace SymbolicRegression
{
using namespace std;
Solver::Solver()
{
    m_populationCount = Config::Instance->GetInt("PopulationCount");
    m_eliteCount = Config::Instance->GetInt("ElitesCount");
    m_reproducer = shared_ptr<Reproducer>(new RandomReproducer(m_populationCount));
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
        auto newExp = Expression::GenerateRandomExpression();
        if (!any_of(m_population.begin(), m_population.end(), [&](const shared_ptr<Expression> &exp) {
                return exp->ToString() == newExp->ToString();
            }))
        {
            m_population.emplace_front(newExp);
        }
    }
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
    }
}

void Solver::SaveOutput()
{
}
} // namespace SymbolicRegression
