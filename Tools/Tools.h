#include <string>
#include <iphlpapi.h>  
#include <ShlDisp.h>
#pragma comment(lib, "IPHLPAPI.lib")
class Tools
{
public:
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
    static bool Unzip2Folder(BSTR lpZipFile, BSTR lpFolder)
    {
        IShellDispatch *pISD;

        Folder  *pZippedFile = 0L;
        Folder  *pDestination = 0L;

        long FilesCount = 0;
        IDispatch* pItem = 0L;
        FolderItems *pFilesInside = 0L;

        VARIANT Options, OutFolder, InZipFile, Item;
        CoInitialize(NULL);
        __try{
            if (CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&pISD) != S_OK)
                return 1;

            InZipFile.vt = VT_BSTR;
            InZipFile.bstrVal = lpZipFile;
            pISD->NameSpace(InZipFile, &pZippedFile);
            if (!pZippedFile)
            {
                pISD->Release();
                return 1;
            }

            OutFolder.vt = VT_BSTR;
            OutFolder.bstrVal = lpFolder;
            GetFileAttributes(lpFolder);

            DWORD attrib = GetFileAttributes(lpFolder);
            if (attrib == INVALID_FILE_ATTRIBUTES)
                CreateDirectory(lpFolder, NULL);


            pISD->NameSpace(OutFolder, &pDestination);
            if (!pDestination)
            {
                pZippedFile->Release();
                pISD->Release();
                return 1;
            }

            pZippedFile->Items(&pFilesInside);
            if (!pFilesInside)
            {
                pDestination->Release();
                pZippedFile->Release();
                pISD->Release();
                return 1;
            }

            pFilesInside->get_Count(&FilesCount);
            if (FilesCount < 1)
            {
                pFilesInside->Release();
                pDestination->Release();
                pZippedFile->Release();
                pISD->Release();
                return 0;
            }


            FolderItem* folder_item = 0L;
            bool retval;
            for (int i = 0; i < FilesCount; i++)
            {
                BSTR name;
                BSTR ext;
                ext = ::SysAllocString(L"ext");
                Item.vt = VT_I4;
                Item.intVal = i;
                pFilesInside->Item(Item, &folder_item);
                folder_item->get_Path(&name);
                size_t length = wcslen(name);
                wcscpy_s(ext, 4, name + length - 3);
                if (wcscmp(ext, L"xml") == 0)
                {
                    folder_item->QueryInterface(IID_IDispatch, (void**)&pItem);
                    Item.vt = VT_DISPATCH;
                    Item.pdispVal = pItem;

                    Options.vt = VT_I4;
                    Options.lVal = 1024 | 512 | 16 | 4;//http://msdn.microsoft.com/en-us/library/bb787866(VS.85).aspx

                    retval = pDestination->CopyHere(Item, Options) == S_OK;
                }
                folder_item->Release(); folder_item = 0;
                ::SysFreeString(ext);
            }

            pItem->Release(); pItem = 0L;
            pFilesInside->Release(); pFilesInside = 0L;
            pDestination->Release(); pDestination = 0L;
            pZippedFile->Release(); pZippedFile = 0L;
            pISD->Release(); pISD = 0L;

            return retval;

        }
        __finally
        {
            CoUninitialize();
        }
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