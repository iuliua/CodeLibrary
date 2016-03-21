#pragma once
class CLogger
{
    const char *m_plugin_name;
    CServerInterface *m_server_api;
public:
    CLogger(const char *plugin_name) :m_plugin_name(plugin_name)
    {}
    void CLogger::SetAPI(CServerInterface *api)
    {
        m_server_api = api;
    }

   void Out(const int code, LPCSTR msg, ...)
    {
        if (msg)
        {
            char buffer[1024];
            va_list ptr;
            va_start(ptr, msg);
            _vsnprintf_s(buffer, sizeof(buffer) - 1, msg, ptr);
            va_end(ptr);
            m_server_api->LogsOut(code, m_plugin_name, buffer);
        }
    }
};
extern CLogger ExtLogger;