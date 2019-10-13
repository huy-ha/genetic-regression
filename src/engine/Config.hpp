#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_
#include <map>
#include <string>
#include <vector>
#include <tuple>
#include <memory>
namespace SymbolicRegression
{
class Config
{
public:
    Config(std::string configFilePath);
    static Config *Instance; // Singleton
    std::vector<std::tuple<float, float>> Data;

public:
    int GetInt(std::string key) const;
    float GetFloat(std::string key) const;
    std::string GetString(std::string key) const;
    operator std::string() const;

private:
    void BadConfigFile(std::string message);
    void ParseConfigLine(std::string configLine);
    bool IsValidKey(std::string key) const;
    void ParseInputDatapoints(std::string inputFilePath);

private:
    std::map<std::string, std::string> m_configs;
};
} // namespace SymbolicRegression
#endif