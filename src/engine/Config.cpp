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
    string buf;
    ifstream configFile;
    configFile.open("configs/" + configFilePath);
    if (!configFile.is_open())
        BadConfigFile("Can't open file");
    while (getline(configFile, buf))
        ParseConfigLine(buf);
    configFile.close();
    cout << string(*this) << endl;
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
        m_configs.insert(pair<string, string>(fields[0], fields[1]));
    else
        BadConfigFile("Invalid Key \"" + fields[0] + "\"");
}

bool Config::IsValidKey(std::string key) const
{
    return std::find(m_configKeys.begin(), m_configKeys.end(), key) != m_configKeys.end();
}

void Config::BadConfigFile(std::string message)
{
    throw std::exception(("Bad Config File: " + message).c_str());
}
} // namespace SymbolicRegression
