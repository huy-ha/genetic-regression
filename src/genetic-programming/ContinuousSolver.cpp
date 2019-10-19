#include "ContinuousSolver.hpp"
#include "../engine/Config.hpp"
#include "../engine/OutputLogger.hpp"
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
        return;
    }
    // select two parents using selector
    auto [p1, p2] = m_selector->Select(m_population);
    // remove parents from population
    m_population.remove(p1);
    m_population.remove(p2);
    // reproduce the two parents, giving at least 2 individuals back as a list
    auto offspring = m_reproducer->CreateOffspring(p1, p2);
    // concat individuals to population
    // TODO experiment with replace most similar parent
    m_population.emplace_front(offspring);
    m_population.emplace_front(p1);
    m_population.emplace_front(p2);
    // handle over population
    m_population.sort(Expression::FitnessComparer);
    while (m_population.size() > m_populationCount)
    {
        m_population.pop_back();
    }
    OutputLogger::Log("HighestFitness", to_string(OutputLogger::GetEvaluations()) + " " + to_string(m_prevHighestFitness));
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
        if (OutputLogger::GetEvaluations() > saveEval * 100)
        {
            SaveOutput();
            saveEval++;
        }
        m_population.sort(Expression::FitnessComparer);
        if ((*m_population.begin())->Fitness() > m_prevHighestFitness)
        {
            m_prevHighestFitness = (*m_population.begin())->Fitness();
            prevBestEvaluations = OutputLogger::GetEvaluations();
            cout << "FITNESS: " << m_prevHighestFitness << " | Temp" << GetTemp() << endl;
            cout << "\t" << (*m_population.begin())->ToString() << endl;
        }
        DecayTemp();
        SavePopulationFitnesses();
    }
    SaveOutput();
    m_population.sort(Expression::FitnessComparer);
    auto finalBest = *m_population.begin();
    cout << "FITNESS " << m_prevHighestFitness
         << " after " << OutputLogger::GetEvaluations() << " evalutions at temp " << GetTemp() << endl;
    cout << "\t" + finalBest->ToString() << endl;
}
} // namespace SymbolicRegression