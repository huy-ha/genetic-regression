#include "OutputLogger.hpp"

namespace SymbolicRegression
{
OutputLogger *OutputLogger::Instance = nulltpr;

OutputLogger::OutputLogger()
{
    Instance = this;
}
} // namespace SymbolicRegression
