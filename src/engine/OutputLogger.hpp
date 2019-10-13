#ifndef _OUTPUT_LOGGER_HPP_
#define _OUTPUT_LOGGER_HPP_
#include <map>
#include <string>
namespace SymbolicRegression
{
class OutputLogger
{
public:
    static OutputLogger *Instance; // Singleton
    static int Evaluations;
    OutputLogger();

private:
    std::map<std::string, std::string> m_log;
};
} // namespace SymbolicRegression
#endif