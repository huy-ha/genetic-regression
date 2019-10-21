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
    if (typeid(*m_reproducer).name() == string("class SymbolicRegression::RandomReproducer"))
    {
        auto offsprings = m_reproducer->Reproduce(m_population);
        m_population.clear();
        for_each(offsprings->begin(), offsprings->end(), [&](auto e) {
            m_population.emplace_front(e);
        });
        OutputLogger::Log("HighestFitness",
                          to_string(OutputLogger::GetEvaluations()) +
                              "," + to_string(m_prevHighestFitness));
        return;
    }
    // select two parents using selector
    auto [p1, p2] = m_selector->Select(m_population);
    // remove parents from population
    m_population.remove(p1);
    m_population.remove(p2);
    // reproduce the two parents, giving at least 2 individuals back as a list
    auto offspring = m_reproducer->CreateOffspring(p1, p2);
    if (!Expression::IsValid(offspring))
    {
        return;
    }
    // concat individuals to population
    if (Expression::RandomF() > 0.5f)
    {
        m_population.emplace_front(offspring);
        m_population.emplace_front(p2);
        m_population.emplace_front(p1);
    }
    else if (Expression::Diversity(offspring, p1) < Expression::Diversity(offspring, p2))
    {
        m_population.emplace_front(offspring);
        m_population.emplace_front(p2);
    }
    else
    {
        m_population.emplace_front(offspring);
        m_population.emplace_front(p1);
    }
    // remove all invalid expressions
    // handle over population
    m_population.sort(Expression::FitnessComparer);
    while (m_population.size() > m_populationCount)
    {
        m_population.pop_back();
    }
    OutputLogger::Log("HighestFitness",
                      to_string(OutputLogger::GetEvaluations()) +
                          "," + to_string(m_prevHighestFitness));
}

void ContinuousSolver::Run()
{
    int prevBestEvaluations = -1;
    m_prevHighestFitness = -1;
    int saveEval = 0;
    InitializePopulation();
    int round = 0;
    // Run for at least 10000 rounds
    // Afterwards, if double evaluations without getting better, quit
    while (round < 10000 ||
           prevBestEvaluations < OutputLogger::GetEvaluations() / 2)
    {
        EvolveRound();
        for_each(m_population.begin(), m_population.end(), [](auto e) {
            e = Expression::Simplify(e);
        });
        // Simplify and remove invalid expressions
        auto it = m_population.begin();
        while ((it = find_if(m_population.begin(), m_population.end(), [](auto e) -> bool {
                    return !Expression::IsValid(e);
                })) != m_population.end())
        {
            m_population.erase(it);
        }
        if (OutputLogger::GetEvaluations() > saveEval * 100)
        {
            SaveOutput();
            saveEval++;
        }
        m_population.sort(Expression::FitnessComparer);
        if ((*m_population.begin())->Fitness() > m_prevHighestFitness)
        {
            m_prevBest = (*m_population.begin());
            m_prevHighestFitness = m_prevBest->Fitness();
            prevBestEvaluations = OutputLogger::GetEvaluations();
            cout << "FITNESS: " << m_prevHighestFitness << " | Temp " << GetTemp() << endl;
            cout << "\t" << m_prevBest->ToString() << endl;
            SaveOutput();
        }
        DecayTemp();
        SavePopulationFitnesses();
        OutputLogger::Log("Diversity",
                          to_string(OutputLogger::GetEvaluations()) +
                              "," + to_string(PopulationDiversity()));
    }
    SaveOutput();
    cout << "FITNESS " << m_prevHighestFitness
         << " after " << OutputLogger::GetEvaluations() << " evalutions at temp " << GetTemp() << endl;
    cout << "\t" + m_prevBest->ToString() << endl;
}
} // namespace SymbolicRegression