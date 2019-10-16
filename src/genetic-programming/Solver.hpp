#ifndef _SOLVER_HPP_
#define _SOLVER_HPP_
#include <list>
#include <memory>
#include <mutex>
#include "reproducers/Reproducer.hpp"
namespace SymbolicRegression
{
using namespace std;
class Solver
{
public:
    Solver();
    void Run();
    void SaveOutput();
    static shared_ptr<Solver> Instance();
    static float GetTemp();
    static void DecayTemp();

protected:
    void InitializePopulation();
    void Evolve();
    void PrintPopulation();

private:
    list<shared_ptr<Expression>> m_population;
    int m_populationCount = -1;
    shared_ptr<Reproducer> m_reproducer;
    int m_eliteCount = -1;
    float m_prevHighestFitness = -1;
    static shared_ptr<Solver> m_instance;
    static float m_temperature;
    static mutex tempMutex;
};
} // namespace SymbolicRegression
#endif