#include <string>
#include <iostream>
#include <fstream>
#include <iphlpapi.h>  
#pragma comment(lib, "IPHLPAPI.lib")
#define CURL_STATICLIB
#include "unzip/unzip.h" 
#include "curl/curl.h"
class Tools
{
public:
    struct MemoryStruct {
        char *memory;
        size_t size;
    };
    static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        size_t realsize = size * nmemb;
        struct MemoryStruct *mem = (struct MemoryStruct *)userp;

        mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
        if (mem->memory == NULL) {
            /* out of memory! */
            printf("not enough memory (realloc returned NULL)\n");
            return 0;
        }

        memcpy(&(mem->memory[mem->size]), contents, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;

        return realsize;
    }
    static bool DownloadLink(std::wstring &link,MemoryStruct& chunk)
    {
        CURL *curl_handle;
        CURLcode res;

        chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
        chunk.size = 0;    /* no data at this point */

        curl_global_init(CURL_GLOBAL_ALL);
        /* init the curl session */
        curl_handle = curl_easy_init();
        /* specify URL to get */
        curl_easy_setopt(curl_handle, CURLOPT_URL, Tools::WStringToString(link).c_str());
        /* send all data to this function  */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &Tools::WriteMemoryCallback);

        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

        /* some servers don't like requests that are made without a user-agent
        field, so we provide one */
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        /* get it! */
        res = curl_easy_perform(curl_handle); 
        /* check for errors */
        if (res != CURLE_OK) {
            //MessageBox(Tools::StringToWString(curl_easy_strerror(res)).c_str());
        }
        else {
            /*
            * Now, our chunk.memory points to a memory block that is chunk.size
            * bytes big and contains the remote file.
            *
            * Do something nice with it!
            */

            //printf("%lu bytes retrieved\n", (long)chunk.size);
        }
        long http_code = 0;
        curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);

        /* cleanup curl stuff */
        curl_easy_cleanup(curl_handle);

        //free(chunk.memory);

        /* we're done with libcurl, so clean it up */
        curl_global_cleanup();
        return (res==CURLE_OK && http_code==200);
    }
    // set current directory to gateway's executable folder location
	static void SetCurrDirToModuleLocation(LPCWSTR folder=NULL)
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
    inline static bool DoubleEqual(double d1, double d2)
    {
        return (abs(d1 - d2) < 0.0001);
    }
    inline static bool DoubleNotZero(double d1)
    {
        return !DoubleEqual(d1, 0);
    }
    inline static bool DoubleEqualZero(double d1)
    {
        return DoubleEqual(d1, 0);
    }
    static bool UnzipFileType(std::wstring lpZipFile,std::wstring out_folder,std::wstring file_type)
    {
        HZIP hz = OpenZip(lpZipFile.c_str(), 0);
        ZIPENTRY ze; GetZipItem(hz, -1, &ze); int numitems = ze.index;
        ZRESULT zr=ZR_FAILED;
        for (int i = 0; i < numitems; i++)
            {
            GetZipItem(hz, i, &ze);
            if (std::wstring(ze.name).substr(std::wstring(ze.name).rfind(L'.') + 1).compare(file_type) == 0)
            {
                char* buffer = new char[ze.unc_size];
                zr=UnzipItem(hz, i, buffer, ze.unc_size);
                std::ofstream out_file(out_folder+L"\\"+ze.name, std::ios::binary);
                out_file.write(buffer, ze.unc_size);
                out_file.close();
            }
            }
        CloseZip(hz);
        return (zr==ZR_OK);
            }
    static bool UnzipFileType(char* buffer, size_t buffer_size,std::wstring file_type, MemoryStruct &out)
            {
        HZIP hz = OpenZip(buffer,(UINT)buffer_size, 0);
        ZIPENTRY ze; GetZipItem(hz, -1, &ze); int numitems = ze.index;
        ZRESULT zr=ZR_FAILED;
        for (int i = 0; i < numitems; i++)
                {
            GetZipItem(hz, i, &ze);
            if (std::wstring(ze.name).substr(std::wstring(ze.name).rfind(L'.') + 1).compare(file_type) == 0)
        {
                out.memory = new char[ze.unc_size];
                out.size = ze.unc_size;
                zr = UnzipItem(hz, i, out.memory, (UINT)out.size);
                //std::ofstream out(out_folder + L"\\" + ze.name, std::ios::binary);
                //out.write(buffer, ze.unc_size);
                //out.close();
            }
        }
        CloseZip(hz);
        return  (zr == ZR_OK);
    }
    static void CalculatePrevDay(SYSTEMTIME st, LPSYSTEMTIME out)
    {
        *out = { 0 };
        int maxday[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        if ((st.wYear & 3) == 0 && ((st.wYear % 25) != 0 || (st.wYear & 15) == 0))
            maxday[2] = 29;

        if (st.wDay == 1)
        {
            if (st.wMonth == 1)
            {
                out->wYear = st.wYear - 1;
                out->wMonth = 12;
                out->wDay = maxday[12];
            }
            else
            {
                out->wYear = st.wYear;
                out->wMonth = st.wMonth - 1;
                out->wDay = maxday[out->wMonth];
            }
        }
        else
        {
            out->wYear = st.wYear;
            out->wMonth = st.wMonth;
            out->wDay = st.wDay - 1;
        }
    }
    static unsigned short getVolumeHash()
    {
        DWORD serialNum = 0;

        // Determine if this volume uses an NTFS file system.      
        GetVolumeInformation(L"c:\\", NULL, 0, &serialNum, NULL, NULL, NULL, 0);
        unsigned short  hash = (unsigned short)((serialNum + (serialNum >> 16)) & 0xFFFF);

        return hash;
    }
    static const char* getMachineName()
    {
        static char computerName[1024];
        DWORD size = 1024;
        GetComputerNameA(computerName, &size);
        return &(computerName[0]);
    }
    static unsigned short getCpuHash()
    {
        int cpuinfo[4] = { 0, 0, 0, 0 };
        __cpuid(cpuinfo, 0);
        unsigned short hash = 0;
        unsigned short* ptr = (unsigned short*)(&cpuinfo[0]);
        for (unsigned i = 0; i < 8; i++)
            hash += ptr[i];

        return hash;
    }
    static unsigned short hashMacAddress(PIP_ADAPTER_INFO info)
    {
        unsigned short hash = 0;
        for (unsigned i = 0; i < info->AddressLength; i++)
        {
            hash += (info->Address[i] << ((i & 1) * 8));
        }
        return hash;
    }

    static void getMacHash(unsigned short& mac1, unsigned short& mac2)
    {
        IP_ADAPTER_INFO AdapterInfo[32];
        DWORD dwBufLen = sizeof(AdapterInfo);

        DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
        if (dwStatus != ERROR_SUCCESS)
            return; // no adapters.      

        PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
        mac1 = hashMacAddress(pAdapterInfo);
        if (pAdapterInfo->Next)
            mac2 = hashMacAddress(pAdapterInfo->Next);

        // sort the mac addresses. We don't want to invalidate     
        // both macs if they just change order.           
        if (mac1 > mac2)
        {
            unsigned short tmp = mac2;
            mac2 = mac1;
            mac1 = tmp;
        }
    }
    static std::string GetSerial()
    {
        unsigned short mac1, mac2;
        getMacHash(mac1, mac2);
        char buffer[13];
        sprintf_s(buffer, 13, "%x%x%x", getVolumeHash(), getCpuHash(), mac1);
        return buffer;
    }
};