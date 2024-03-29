#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
namespace SymbolicRegression
{
using namespace std;

Config *Config::Instance = nullptr;
shared_ptr<vector<tuple<float, float>>> Config::Data(new vector<tuple<float, float>>());

Config::Config(string configFilePath, string outputFilePath)
{
    m_configs.insert(make_pair("Input", "data.txt"));

    m_configs.insert(make_pair("Solver", "Continuous"));
    m_configs.insert(make_pair("Reproducer", "CrossoverMutator"));
    m_configs.insert(make_pair("Selector", "Diversity"));
    m_configs.insert(make_pair("MinThreads", "20"));
    m_configs.insert(make_pair("MaxThreads", "-1"));
    m_configs.insert(make_pair("MaxDepth", "5"));

    m_configs.insert(make_pair("Init_T", "0.5"));
    m_configs.insert(make_pair("T_decay", "0.999999"));
    m_configs.insert(make_pair("MutationRetries", "1"));

    m_configs.insert(make_pair("PopulationCount", "100"));
    m_configs.insert(make_pair("GenerationCount", "50000"));
    m_configs.insert(make_pair("ElitesCount", "1"));
    m_configs.insert(make_pair("DotPlot", "1"));
    m_configs.insert(make_pair("OutputPath", "runs/" + outputFilePath + "/"));

    m_configs.insert(make_pair("TournamentPlayersCount", "4"));
    Instance = this;
    if (configFilePath != ".config")
    {
        string buf;
        ifstream configFile;
        configFile.open("configs/" + configFilePath);
        if (!configFile.is_open())
            BadConfigFile("Can't open file");
        while (getline(configFile, buf))
            ParseConfigLine(buf);
        configFile.close();
    }
    ParseInputDatapoints(m_configs["Input"]);
    // UseTestInputDatapoints();
}

void Config::UseTestInputDatapoints()
{
    auto f1 = [](float x) {
        return x;
    };
    auto f2 = [](float x) {
        return cosf(x) * 1.5f;
    };
    for (float i = 0; i < 10.0f; i += 0.01f)
    {
        Data->push_back(tuple<float, float>(i, f2(i)));
    }
}

void Config::ParseInputDatapoints(string inputFilePath)
{
    ifstream inputFile;
    inputFile.open("inputs/" + inputFilePath);
    if (!inputFile.is_open())
        BadConfigFile("Can't open file");
    vector<string> lines;
    copy(istream_iterator<string>(inputFile), istream_iterator<string>(), back_inserter(lines));
    if (lines.size() % 2 != 0)
        BadConfigFile("Input Data in wrong format");
    for (int i = 0; i < lines.size() / 2; i++)
    {
        Data->push_back(tuple<float, float>(stof(lines[2 * i]), stof(lines[2 * i + 1])));
    }
}

int Config::GetInt(std::string key)
{
    if (IsValidKey(key))
    {
        return stoi(Instance->m_configs.at(key));
    }
    cout << "Invalid Key" + key << endl;
    throw std::exception(("Invalid Key" + key).c_str());
}
float Config::GetFloat(std::string key)
{
    if (IsValidKey(key))
    {
        return stof(Instance->m_configs.at(key));
    }
    cout << "Invalid Key" + key << endl;
    throw std::exception(("Invalid Key" + key).c_str());
}
std::string Config::GetString(std::string key)
{
    if (IsValidKey(key))
    {
        return Instance->m_configs.at(key);
    }
    cout << "Invalid Key" + key << endl;
    throw std::exception(("Invalid Key" + key).c_str());
}

Config::operator string() const
{
    string output = "";
    for (map<string, string>::const_iterator it = m_configs.begin(); it != m_configs.end(); ++it)
    {
        output += it->first + ":" + it->second + "\n";
    }
    return output;
}
void Config::ParseConfigLine(string configLine)
{

    vector<string> fields{
        istream_iterator<string>(stringstream(configLine)),
        istream_iterator<string>()};
    if (fields.size() != 2)
        BadConfigFile("Wrong format \"" + configLine + "\"");
    if (IsValidKey(fields[0]))
        m_configs[fields[0]] = fields[1];
    else
        BadConfigFile("Invalid Key \"" + fields[0] + "\"");
}

bool Config::IsValidKey(string key)
{
    return Instance->m_configs.find(key) != Instance->m_configs.end();
}

void Config::BadConfigFile(string message)
{
    throw exception(("Bad Config File: " + message).c_str());
}
} // namespace SymbolicRegression
