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