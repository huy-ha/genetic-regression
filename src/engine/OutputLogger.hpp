#ifndef _OUTPUT_LOGGER_HPP_
#define _OUTPUT_LOGGER_HPP_
#include <map>
#include <string>
#include <memory>
#include <mutex>
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
    static void IncrementEvaluations();
    static int GetEvaluations();

private:
    map<string, string> m_log;
    static shared_ptr<OutputLogger> m_instance;
    static int Evaluations;
    static mutex evalMu;
};
} // namespace SymbolicRegression
#endif