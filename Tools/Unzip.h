#pragma once
#include "CommonTools.h"
#include "StringConversion.h"
#include "zip/unzip.h" 
namespace Tools
{
    static bool UnzipFileType(std::wstring lpZipFile, std::wstring out_folder, std::wstring file_type)
    {
        HZIP hz = OpenZip(lpZipFile.c_str(), 0);
        ZIPENTRY ze; GetZipItem(hz, -1, &ze); int numitems = ze.index;
        ZRESULT zr = ZR_FAILED;
        for (int i = 0; i < numitems; i++)
        {
            GetZipItem(hz, i, &ze);
            if (std::wstring(ze.name).substr(std::wstring(ze.name).rfind(L'.') + 1).compare(file_type) == 0)
            {
                char* buffer = new char[ze.unc_size];
                zr = UnzipItem(hz, i, buffer, ze.unc_size);
                std::ofstream out_file(out_folder + L"\\" + ze.name, std::ios::binary);
                out_file.write(buffer, ze.unc_size);
                out_file.close();
                delete[] buffer;
            }
        }
        CloseZip(hz);
        return (zr == ZR_OK);
    }
    static bool UnzipFileType(char* buffer, size_t buffer_size, std::wstring file_type, MemoryStruct &out)
    {
        HZIP hz = OpenZip(buffer, (UINT)buffer_size, 0);
        ZIPENTRY ze; GetZipItem(hz, -1, &ze); int numitems = ze.index;
        ZRESULT zr = ZR_FAILED;
        for (int i = 0; i < numitems; i++)
        {
            GetZipItem(hz, i, &ze);
            if (std::wstring(ze.name).substr(std::wstring(ze.name).rfind(L'.') + 1).compare(file_type) == 0)
            {
                out.memory = new char[ze.unc_size];
                out.size = ze.unc_size;
                zr = UnzipItem(hz, i, out.memory, (UINT)out.size);
            }
        }
        CloseZip(hz);
        return  (zr == ZR_OK);
    }
}