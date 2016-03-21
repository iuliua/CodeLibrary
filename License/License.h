#include <string>
class CLicense
{
public:
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
#ifdef MTServerAPIVersion
    static bool Check(IMTServerAPI* api, UINT owner_name_hash)
    {
        if (api)
        {
            MTServerInfo srvinfo;
            if (api->About(srvinfo) == MT_RET_OK)
                if (Hash(srvinfo.platform_owner) == owner_name_hash)
                    return true;
        }
        return false;
    }
#endif
#ifdef ServerApiVersion
    static bool Check(CServerInterface* api, UINT owner_name_hash)
    {
        if (api)
        {
            ConCommon info = { 0 };
            api->CommonGet(&info);
            std::string owner(info.owner);
            std::string name_only = owner.substr(0, owner.find_first_of('(') - 1);
            if (Hash(name_only.c_str()) == owner_name_hash)
                return true;
        }
        return false;
    }
#endif
};