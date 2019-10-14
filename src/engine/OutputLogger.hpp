#ifndef _OUTPUT_LOGGER_HPP_
#define _OUTPUT_LOGGER_HPP_
#include <map>
#include <string>
#include <memory>
namespace SymbolicRegression
{
using namespace std;
class OutputLogger
{
public:
    OutputLogger();
    static void Log(string key, string log);
    static string Get(string key);
    static shared_ptr<OutputLogger> Instance(); // Singleton
    static int Evaluations;

private:
    map<string, string> m_log;
    static shared_ptr<OutputLogger> m_instance;
};
} // namespace SymbolicRegression
#endif