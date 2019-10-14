#include "OutputLogger.hpp"
#include <memory>
namespace SymbolicRegression
{
using namespace std;

int OutputLogger::Evaluations = 0;
shared_ptr<OutputLogger> OutputLogger::m_instance(new OutputLogger());
mutex OutputLogger::evalMu;

shared_ptr<OutputLogger> OutputLogger::Instance()
{
    return m_instance;
}

OutputLogger::OutputLogger()
{
    m_log.insert(pair<string, string>("HighestFitness", ""));
}

int OutputLogger::GetEvaluations()
{
    lock_guard<mutex> lock(evalMu);
    return Evaluations;
}

void OutputLogger::IncrementEvaluations()
{
    lock_guard<mutex> lock(evalMu);
    Evaluations = Evaluations + 1;
}

void OutputLogger::Log(string key, string log)
{
    m_instance->m_log[key] += log + "\n";
}

string OutputLogger::Get(string key)
{
    return m_instance->m_log[key];
}
} // namespace SymbolicRegression
