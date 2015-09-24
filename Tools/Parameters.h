#pragma once
#include <string>
#include <iostream>
#include <fstream>
namespace Tools
{
class CParameters
{
private:
    std::map<std::string, std::string> m_parameters;
    std::vector<std::string> m_names;
    std::string m_config_file;
public:
    CParameters() {}
    int total() { return m_parameters.size(); }
    int set(const char* name, const char*value)
    {
        if (std::find(m_names.begin(), m_names.end(), name) != m_names.end())
        {
            m_parameters[name] = value;
            return true;
        }
        else
            return false;
    }
    int next(const int index, char* name,char*value)
    {
        if (index < (int)m_names.size())
        {
            strcpy(name, m_names[index].c_str());
            strcpy(value, m_parameters[m_names[index]].c_str());
            return true;
        }
        else
            return false;
    }
    std::string &get(std::string param_name)
    { 
        return m_parameters[param_name]; 
    }
    void set_param(std::string param_name, std::string default_value)
    {
        m_names.push_back(param_name);
        m_parameters[param_name] = default_value;
    }
    void LoadFromFile(const std::string &config_file)
    {
        std::ifstream ifs_config_file(config_file);
        m_config_file = config_file;
        if (ifs_config_file)
        {
            std::string line;
            do
            {
                std::getline(ifs_config_file, line);
                if (line.size())
                {
                    std::istringstream ss(line);
                    std::string param, value;
                    std::getline(ss, param, '=');
                    std::getline(ss, value, '=');
                    m_parameters[param] = value;
                }
            } while (!ifs_config_file.eof());
        }
    }
    void WriteToDisk()
    {
        std::ofstream ofs_config_file(m_config_file);
        for (std::string &param : m_names)
        {
            ofs_config_file << param <<"="<< m_parameters[param] << std::endl;
        }
        ofs_config_file.close();
    }
};
}