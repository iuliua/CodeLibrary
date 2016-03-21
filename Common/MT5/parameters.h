#pragma once
#include "Tools/Matching.h"
#include "Tools/StringConversion.h"

#define PARAM_SYM L"symbols"
#define PARAM_GRP L"groups"
#define PARAM_DELAY L"delay"
#define PARAM_TAX L"tax"
#define PARAM_COMMENT L"comment"
#define PARAM_COMMISSION L"commission"
#define PARAM_MAN L"managers"
#define PARAM_MASTER L"master"

class CParameters
{
   Tools::CMatching  m_sym_matching;
   Tools::CMatching  m_grp_matching;
   Tools::CMatching  m_man_matching;
   std::wstring      m_param_delay;
   std::wstring      m_param_tax;
   std::wstring      m_param_comment;
   std::wstring      m_param_commission;
   std::wstring      m_param_master;
   std::vector<MTPluginParam> m_params;
   
public:
    CParameters()
    {
        m_params.push_back({MTPluginParam::TYPE_STRING, PARAM_SYM, L"*"});
        m_params.push_back({MTPluginParam::TYPE_STRING, PARAM_GRP, L"*"});
        m_params.push_back({MTPluginParam::TYPE_STRING, PARAM_DELAY, L"20"});
        m_params.push_back({MTPluginParam::TYPE_STRING, PARAM_TAX, L"0" });
        m_params.push_back({MTPluginParam::TYPE_STRING, PARAM_COMMENT, L"Capital Tax Gains"});
        m_params.push_back({ MTPluginParam::TYPE_STRING, PARAM_COMMISSION, L"0"});
        m_params.push_back({MTPluginParam::TYPE_STRING, PARAM_MAN, L"managers\\*"}); 
        m_params.push_back({MTPluginParam::TYPE_STRING, PARAM_MASTER, L""});
    }
    void ParamProcess(const std::wstring &name, const std::wstring &value)
    {
        if (name == PARAM_GRP)
            m_grp_matching.Load(Tools::WStringToString(value));
        else
            if (name == PARAM_SYM)
                m_sym_matching.Load(Tools::WStringToString(value));
            else
                if (name == PARAM_DELAY)
                    m_param_delay = value.c_str();
                else
                    if (name == PARAM_TAX)
                        m_param_tax = value.c_str();
                    else
                        if (name == PARAM_COMMENT)
                            m_param_comment = value.c_str();
                        else
                            if (name == PARAM_COMMISSION)
                                m_param_commission = value.c_str();
                            else
                                if (name == PARAM_MAN)
                                    m_man_matching.Load(Tools::WStringToString(value));
                                else
                                    if (name == PARAM_MASTER)
                                        m_param_master = value.c_str();
                                    else
                                        ExtLogger.Out(MTLogErr, L"invalid parameter %s", name.c_str());
    }
public:
    bool Match(const std::wstring &name, const std::wstring& value)
    {
        if (name == PARAM_GRP)
            return m_grp_matching.Match(Tools::WStringToString(value));
        else
            if (name == PARAM_SYM)
                return m_sym_matching.Match(Tools::WStringToString(value));
            else
                if (name == PARAM_MAN)
                    return m_man_matching.Match(Tools::WStringToString(value));
        return false;
    }
    std::wstring ParamValue(const std::wstring &name)
    {
        if (name == PARAM_DELAY)
            return m_param_delay;
        else
            if (name == PARAM_COMMENT)
                return m_param_comment;
            else
                if (name == PARAM_TAX)
                    return m_param_tax;
                else
                    if (name == PARAM_COMMISSION)
                        return m_param_commission;
                    else
                        if (name == PARAM_MASTER)
                            return m_param_master;
        return L"";
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
                ParamProcess(std::wstring(param->Name()), param->Value());
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
        info.defaults_total = static_cast<UINT>(m_params.size());
        for (UINT i = 0; i < info.defaults_total; i++)
            info.defaults[i] = m_params[i];
    }
};