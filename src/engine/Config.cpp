#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
namespace SymbolicRegression
{
Config::Config(std::string configFilePath)
{
    using namespace std;
    m_configs.insert(pair<string, string>("Input", "data.txt"));
    m_configs.insert(pair<string, string>("PopulationCount", "100"));
    m_configs.insert(pair<string, string>("GenerationCount", "50000"));
    // Instance = this;
    string buf;
    ifstream configFile;
    configFile.open("configs/" + configFilePath);
    if (!configFile.is_open())
        BadConfigFile("Can't open file");
    while (getline(configFile, buf))
        ParseConfigLine(buf);
    configFile.close();
    cout << string(*this) << endl;
    ParseInputDatapoints(m_configs["Input"]);
}

void Config::ParseInputDatapoints(std::string inputFilePath)
{
    using namespace std;
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
        Data.push_back(tuple<float, float>(stof(lines[2 * i]), stof(lines[2 * i + 1])));
    }
}

Config::operator std::string() const
{
    using namespace std;
    string output = "";
    for (map<string, string>::const_iterator it = m_configs.begin(); it != m_configs.end(); ++it)
    {
        output += it->first + ":" + it->second + "\n";
    }
    return output;
}
void Config::ParseConfigLine(std::string configLine)
{
    using namespace std;
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

bool Config::IsValidKey(std::string key) const
{
    return m_configs.find(key) != m_configs.end();
}

void Config::BadConfigFile(std::string message)
{
    throw std::exception(("Bad Config File: " + message).c_str());
}
} // namespace SymbolicRegression
