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
    // set current directory to gateway's executable folder location
    static void SetCurrDirToModuleLocation(LPCWSTR folder = NULL)
    {
        WCHAR buffer[MAX_PATH];
        GetModuleFileName(GetModuleHandle(NULL), buffer, _countof(buffer));
        std::wstring module_name(buffer);
        module_name = module_name.substr(0, module_name.rfind(L'\\'));
        if (folder)
            module_name = module_name + L"\\" + std::wstring(folder);
        SetCurrentDirectory(module_name.c_str());
    }
    static UINT GetNumberOfDigits(double number)
    {
        char buf[32];
        char* pointer = buf;
        UINT digits = 0;
        bool start_counting = false;
        sprintf_s(buf, 32, "%f", number);
        while (*pointer)
        {
            if (*pointer == '.')
            {
                start_counting = true;
                pointer++;
                digits++;
                continue;
            }
            if (*pointer == '0')
            {
                if (start_counting)
                    digits++;
                pointer++;
            }
            else
                return digits;
        }
        return 0;
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
}