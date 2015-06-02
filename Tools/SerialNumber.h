#pragma once
#include <string>
#include <iphlpapi.h>  
#pragma comment(lib, "IPHLPAPI.lib")
namespace Tools
{
    static unsigned short getVolumeHash()
    {
        DWORD serialNum = 0;
        // Determine if this volume uses an NTFS file system.      
        GetVolumeInformation(L"c:\\", NULL, 0, &serialNum, NULL, NULL, NULL, 0);
        unsigned short  hash = (unsigned short)((serialNum + (serialNum >> 16)) & 0xFFFF);
        return hash;
    }
    static unsigned short getMachineNameHash()
    {
        char computerName[1024];
        DWORD size = 1024;
        computerName[0] = 0;
        GetComputerNameA(computerName, &size);
        unsigned short hash = 0;
        for (unsigned i = 0; i < strlen(computerName); i++)
            hash += computerName[i];
        return hash;
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
        mac1 = 0; mac2 = 0;
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
        char buffer[64];
        sprintf_s(buffer, 64, "%04x%04x%04x%04x", getVolumeHash(), getCpuHash(), mac1==0?mac2:mac1,getMachineNameHash());
        return buffer;
    }
};
    