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