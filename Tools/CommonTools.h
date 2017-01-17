#pragma once
#include <string>
#include <iostream>
#include <fstream>
namespace Tools
{
    struct MemoryStruct
    {
        char *memory;
        size_t size;
    };
    // set current directory to modules' location 
    static void SetCurrDirToModuleLocation(HANDLE module,LPCWSTR folder = NULL)
    {
        WCHAR buffer[MAX_PATH];
        GetModuleFileNameW((HMODULE)module, buffer, _countof(buffer));
        std::wstring module_name(buffer);
        module_name = module_name.substr(0, module_name.rfind(L'\\'));
        if (folder)
            module_name = module_name + L"\\" + std::wstring(folder);
        SetCurrentDirectoryW(module_name.c_str());
    }
	// set current directory to files' location 
	static void SetCurrDirToFileLocation(LPCWSTR file)
	{
		std::wstring file_path(file);
		file_path = file_path.substr(0, file_path.rfind(L'\\'));
		SetCurrentDirectoryW(file_path.c_str());
	}
    // counts digits for number of form 0.0001
    static UINT GetNumberOfDigits(double number)
    {
        char buf[32];
        sprintf_s(buf, 32, "%f", number);
        UINT index = (UINT)strlen(buf) - 1 - 2;
        char *pointer = buf + index;
        while (*pointer)
        {
            if (*pointer == '0')
            {
                index--;
                pointer--;
            }
            else
                break;
        }
        return index - 1;
    }
    static std::string ToString(int number)
    {
        char buffer[64];
		if (_itoa_s(number, buffer, _countof(buffer), 10) == 0)
			return buffer;
		else
			return std::string();
    }
    static void CalculatePrevDay(SYSTEMTIME &st)
    {
        int maxday[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        if ((st.wYear & 3) == 0 && ((st.wYear % 25) != 0 || (st.wYear & 15) == 0))
            maxday[2] = 29;

        if (st.wDay == 1)
        {
            if (st.wMonth == 1)
            {
                st.wYear = st.wYear - 1;
                st.wMonth = 12;
                st.wDay = maxday[12];
            }
            else
            {
                st.wYear = st.wYear;
                st.wMonth = st.wMonth - 1;
                st.wDay = maxday[st.wMonth];
            }
        }
        else
        {
            st.wYear = st.wYear;
            st.wMonth = st.wMonth;
            st.wDay = st.wDay - 1;
        }
    }
    class CMeasureTime
    {
        LARGE_INTEGER m_start, finish, freq;
    public:
        CMeasureTime() { QueryPerformanceFrequency(&freq); }
        void start()
        {
            QueryPerformanceCounter(&m_start);
        }
        double stop()
        {
            QueryPerformanceCounter(&finish);
            return (finish.QuadPart - m_start.QuadPart) / (double)freq.QuadPart;
        }
    };
    static UINT Hash(LPCWSTR str)
    {
        unsigned hash = 2197520818;
        if (str)
        {
            while (*str)
            {
                hash = hash * 101 + *str++;
            }
        }
        return hash;
    }
    static UINT Hash(LPCSTR str)
    {
        wchar_t dst[128];
        if (!::MultiByteToWideChar(CP_ACP, 0, str, -1, dst, _countof(dst)))
            dst[0] = 0;
        return Hash(dst);
    }
}
