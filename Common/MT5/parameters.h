#pragma once
#include <map>
#include <vector>
#include "Tools/Matching.h"
#include "Tools/StringConversion.h"

class IParams
{
public:
    virtual std::string get(const std::string&) = 0;
    virtual void Initialize(IMTServerAPI *api) = 0;
};

class CParameters:public IParams
{
public:
    typedef std::vector < std::pair < std::string, std::string>> ParamList;

private:
    ParamList         m_param_list;
    std::map<std::string, std::string> m_param_values;

public:
    CParameters(const ParamList& param_list)
    {
        m_param_list = param_list;
    }

public:
    std::string get(const std::string &param_name)
    {
        return m_param_values[param_name];
    }

public:
    void Initialize(IMTServerAPI* api)
    {
        if (api == NULL)
            return;
        IMTConPlugin* plugin = api->PluginCreate();
        IMTConParam*  param = api->PluginParamCreate();
        if (plugin == NULL || param == NULL)
            return;
        if (api->PluginCurrent(plugin) == MT_RET_OK)
            for (UINT i = 0; plugin->ParameterNext(i, param) == MT_RET_OK; i++)
                m_param_values[Tools::WStringToString(param->Name())] = Tools::WStringToString(param->Value());
        param->Release();
        plugin->Release();
    }
    void About(MTPluginInfo& info)
    {
        info.version = PLUGIN_VERSION;
        info.version_api = MTServerAPIVersion;
        CMTStr::Copy(info.name, _countof(info.name), PLUGIN_NAME);
        CMTStr::Copy(info.copyright, _countof(info.copyright), COPYRIGHT);
        CMTStr::Copy(info.description, _countof(info.description), PLUGIN_DESCRIPTION);
        info.defaults_total = static_cast<UINT>(m_param_list.size());
        for (UINT i = 0; i < info.defaults_total; i++)
        {
            wcscpy_s(info.defaults[i].name, _countof(info.defaults[i].name), Tools::StringToWString(m_param_list[i].first).c_str());
            wcscpy_s(info.defaults[i].value, _countof(info.defaults[i].value), Tools::StringToWString(m_param_list[i].second).c_str());
            info.defaults[i].type = MTPluginParam::TYPE_STRING;
        }
    }
};