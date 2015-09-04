#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
namespace Tools{
class CMatching
{
private:
    std::vector<std::string> m_pattern_list;
    bool MatchExact(const std::string &pattern, const std::string &input)
    {
        if (pattern.compare("*") == 0 || pattern.compare(input) == 0)
            return true;
        return false;
    }

    bool MatchWild(const std::string &pattern, const std::string &input)
    {
        if (pattern[pattern.size() - 1] == '*' && pattern[pattern.size() - 2] == '\\')
        {
            std::string pattern_path = pattern.substr(0, pattern.length() - 1);
            if (pattern_path.size() > 0)
            {
                std::string symb_path(input);
                if (symb_path.find(pattern_path) == 0)
                    return true;
            }
        }
        return false;
    }
public:
    CMatching(){};
    ~CMatching(){};
    void Load(const std::string &pattern, bool demo_mode=false)
    {
        std::string          item;
        std::istringstream   ss(pattern);
        m_pattern_list.clear();
        while (std::getline(ss, item, ','))
        {
            if (demo_mode)
            {
                // demo mode accepts only 2 symbols 
                if (item[item.size() - 1] == L'*')
                    continue;
                if (m_pattern_list.size() == 2)
                    break;
            }
            m_pattern_list.push_back(item);
        }
    }

    bool Match(const std::string &input)
    {
        for (size_t i = 0; i < m_pattern_list.size(); i++)
            if (MatchExact(m_pattern_list[i], input) || MatchWild(m_pattern_list[i], input))
                return true;
        return false;
    }
};
}
