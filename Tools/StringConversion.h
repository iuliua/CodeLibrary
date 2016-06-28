#pragma once
#include <string>
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
};