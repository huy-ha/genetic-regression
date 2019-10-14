#include "OutputLogger.hpp"
#include <memory>
namespace SymbolicRegression
{
using namespace std;

int OutputLogger::Evaluations = 0;
shared_ptr<OutputLogger> OutputLogger::m_instance(new OutputLogger());
shared_ptr<OutputLogger> OutputLogger::Instance()
{
    return m_instance;
}

OutputLogger::OutputLogger()
{
    m_log.insert(pair<string, string>("HighestFitness", ""));
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
