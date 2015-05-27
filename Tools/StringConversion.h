#pragma once
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
};