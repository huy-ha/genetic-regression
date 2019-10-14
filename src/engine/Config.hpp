#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_
#include <map>
#include <string>
#include <vector>
#include <tuple>
#include <memory>
namespace SymbolicRegression
{
using namespace std;
class Config
{
public:
    Config(string configFilePath, string outputFilePath);
    static Config *Instance; // Singleton
    static shared_ptr<vector<tuple<float, float>>> Data;

public:
    static int GetInt(string key);
    static float GetFloat(string key);
    static string GetString(string key);
    operator string() const;

private:
    void BadConfigFile(string message);
    void ParseConfigLine(string configLine);
    static bool IsValidKey(string key);
    void ParseInputDatapoints(string inputFilePath);

protected:
    map<string, string> m_configs;
};
} // namespace SymbolicRegression
#endif