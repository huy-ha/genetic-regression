#include "OutputLogger.hpp"
#include <memory>
#include <iostream>
#include <algorithm>
namespace SymbolicRegression
{
using namespace std;

int OutputLogger::Evaluations = 0;
shared_ptr<OutputLogger> OutputLogger::m_instance(new OutputLogger());
mutex OutputLogger::evalMu;

shared_ptr<OutputLogger> OutputLogger::Instance()
{
    return m_instance;
}

OutputLogger::OutputLogger()
{
    m_log.insert(pair<string, string>("Config", ""));
    m_log.insert(pair<string, string>("HighestFitness", ""));
    m_log.insert(pair<string, string>("FinalBest", ""));
}

shared_ptr<vector<string>> OutputLogger::GetKeys()
{
    shared_ptr<vector<string>> keys(new vector<string>());
    keys->reserve(m_instance->m_log.size());
    for (auto const &it_log : m_instance->m_log)
        keys->push_back(it_log.first);
    return keys;
}

void OutputLogger::Clear(string key)
{
    m_instance->m_log[key] = "";
}

int OutputLogger::GetEvaluations()
{
    lock_guard<mutex> lock(evalMu);
    return Evaluations;
}

void OutputLogger::IncrementEvaluations()
{
    lock_guard<mutex> lock(evalMu);
    Evaluations = Evaluations + 1;
}

void OutputLogger::Log(string key, string log)
{
    m_instance->m_log[key] += log + "\n";
}

string OutputLogger::Get(string key)
{
    return m_instance->m_log[key];
}
} // namespace SymbolicRegression
