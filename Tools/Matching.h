#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
namespace Tools{
    /*
       Supported wildcard:
          - exact match      EURUSD will match EURUSD
          - wildcard match   EUR*   will match EURUSD,EURAUD,etc
                             *USD   will match EURUSD,JPYUSD, etc
                             *US*   will match EURUSD
                                    will NOT match USDJPY
                             *AB*C* NOT SUPPORTED
          - negative match  !*EUR*  will match everything that doesn't contain EUR
    */
class CMatching
{
private:
    std::vector<std::string> m_pattern_list;
    bool MatchExact(const std::string &pattern, const std::string &input)
    {
        if (pattern.size() > 0)
        if (pattern.compare("*") == 0 || pattern.compare(input) == 0)
            return true;
        return false;
    }

    bool MatchWild(const std::string &pattern, const std::string &input)
    {
        if (pattern.size() == 0)
            return false;
        if (pattern.size() == 1 && pattern[0]=='*')
            return true;
        if (pattern[pattern.size() - 1] == '*')
        {
            std::string pattern_path = pattern.substr(0, pattern.length() - 1);
            if (input.find(pattern_path) == 0)
                return true;
        }
        if (pattern[0] == '*')
        {
            std::string pattern_path = pattern.substr(1);
            size_t index=input.find(pattern_path);
            if (index != -1 &&  index==(input.length()-pattern_path.length()))
                return true;
        }
        if (pattern[0] == '*' && pattern[pattern.size() - 1] == '*')
        {
            std::string pattern_path = pattern.substr(1,pattern.length()-2);
            if (input.find(pattern_path) != -1)
                return true;
        }
        return false;
    }
    bool MatchNegative(const std::string &pattern, const std::string &input)
    {
        if (pattern.size() > 0)
        if (pattern[0] == '!')
        {
            std::string pattern_negative = pattern.substr(1);
            if (!MatchExact(pattern_negative, input) && !MatchWild(pattern_negative, input))
               return true;
        }
        return false;
    }
public:
    CMatching(){};
    ~CMatching(){};
    void Load(const std::string &pattern)
    {
        std::string          item;
        std::istringstream   ss(pattern);
        m_pattern_list.clear();
        while (std::getline(ss, item, ','))
            m_pattern_list.push_back(item);
    }

    bool Match(const std::string &input)
    {
        for (size_t i = 0; i < m_pattern_list.size(); i++)
            if (MatchExact(m_pattern_list[i], input) || MatchWild(m_pattern_list[i], input) || MatchNegative(m_pattern_list[i], input))
                return true;
        return false;
    }
};
}
