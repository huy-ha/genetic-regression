#include "OutputLogger.hpp"

namespace SymbolicRegression
{
using namespace std;
OutputLogger *OutputLogger::Instance = nullptr;
int OutputLogger::Evaluations = 0;

OutputLogger::OutputLogger()
{
    Instance = this;
    m_log.insert(pair<string, string>("HighestFitness", ""));
}
} // namespace SymbolicRegression
