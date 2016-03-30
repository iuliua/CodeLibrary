#pragma once

#include <string>
#include <iostream>
#include <fstream>

namespace Tools
{
    class IParams
    {
    public:
        virtual std::string get(const std::string&) = 0;
    };

    class CParameters :public IParams
    {

    private:
        std::map<std::string, std::string> m_parameters;
        std::vector<std::string> m_names;
        std::string m_config_file;

    public:
        CParameters(const char* config_file_name)
        {
            m_config_file = std::string(config_file_name) + ".cfg";
        }

        int total() { return m_parameters.size(); }

        operator IParams*() { return this; }

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

        int next(const int index, char* name, char*value)
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

        virtual std::string get(const std::string &param_name) override
        {
            return m_parameters[param_name];
        }

        void set_param(std::string param_name, std::string default_value)
        {
            m_names.push_back(param_name);
            m_parameters[param_name] = default_value;
        }

        void LoadFromFile()
        {
            std::string   line;
            std::ifstream ifs_config_file(m_config_file);
            if (!ifs_config_file)
                return;
            while (!ifs_config_file.eof())
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
            }
        }

        void WriteToDisk()
        {
            std::ofstream ofs_config_file(m_config_file);
            for (std::string &param : m_names)
                ofs_config_file << param << "=" << m_parameters[param] << std::endl;
        }
    };
}