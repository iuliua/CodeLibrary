#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
namespace Tools
{
    static const std::wstring StringToWString(const std::string& s)
    {
        wchar_t dst[128];
        if (!::MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, dst, _countof(dst)))
            dst[0] = 0;
        return dst;
    }
    static std::string WStringToString(const std::wstring& s)
    {
        char dst[128];
        if (!::WideCharToMultiByte(CP_ACP, 0, s.c_str(), -1, dst, _countof(dst), NULL, NULL))
            dst[0] = 0;
        return dst;
    }
    static std::string ToString(INT64 number)
    {
        char buffer[64];
        _i64toa_s(number, buffer,64,10);
        return buffer;
    }
    static std::string ToString(UINT64 number)
    {
        char buffer[64];
        _i64toa_s(number, buffer, 64, 10);
        return buffer;
    }
    static std::wstring ToWString(INT64 number)
    {
        wchar_t buffer[64];
        _i64tow_s(number, buffer,64,10);
        return buffer;
    }
    static inline std::string Trim(const std::string &s)
    {
        auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c){return std::isspace(c); });
        auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c){return std::isspace(c); }).base();
        return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
    }
    static void split(const std::string &s, char delim, std::vector<std::string> &elems)
    {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim))
            elems.push_back(item);
    }
    static bool iequals(const std::string& a, const std::string& b)
    {
        size_t sz = a.size();
        if (b.size() != sz)
            return false;
        for (unsigned int i = 0; i < sz; ++i)
            if (tolower(a[i]) != tolower(b[i]))
                return false;
        return true;
    }
};
class ILineHandler
{
public:
    virtual void Init() = 0;
    virtual void Handle(const std::string&) = 0;
    operator ILineHandler*()
    {
        return this;
    }
};
class CTextFileProcessor
{
    std::string m_file_name;
public:
    CTextFileProcessor(const std::string &file_name)
        :m_file_name(file_name)
    {

    }
    CTextFileProcessor(const std::wstring &file_name)
        :m_file_name(Tools::WStringToString(file_name))
    {

    }
    BOOL Process(ILineHandler* handler)
    {
        std::ifstream fs(m_file_name);
        if (!fs)
            return FALSE;
        std::string line;
        while (TRUE)
        {
            line.clear();
            std::getline(fs, line);
            if (line.empty())
                break;
            handler->Handle(line);
        };
        return TRUE;
    }
};