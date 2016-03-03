//+------------------------------------------------------------------+
//|                                       MetaTrader 5 TFEX Gateway  |
//|                       Copyright 2001-2015, MetaTrader Solutions  |
//|                              http://www.metatradersolutions.com  |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Journal class                                                    |
//+------------------------------------------------------------------+
class CLogger
{

private:
    //--- constants
    enum constants
    {
        PREBUF_SIZE = 16 * 1024                     // size of a preliminary buffer
    };
    //--- pointer to the gateway interface
    IMTServerAPI    *m_api_gateway;
    //--- message buffer
    wchar_t           m_prebuf[PREBUF_SIZE];
    //--- synchronizer
    CMTSync           m_sync;

public:
    //--- constructor/destructor
    CLogger() : m_api_gateway(NULL)
    {
        //--- clear the buffer
        ZeroMemory(m_prebuf, sizeof(m_prebuf));
    }
    ~CLogger(){};
    //--- basic functional
    void              SetGatewayAPI(IMTServerAPI *api_gateway)
    {
        //--- lock
        m_sync.Lock();
        //--- remember the gateway interface
        m_api_gateway = api_gateway;
        //--- unlock
        m_sync.Unlock();
    }
    void              Out(int code, LPCWSTR format, ...)
    {
        va_list arg_ptr;
        //--- lock
        m_sync.Lock();
        //--- if there is a gateway interface, record an entry to the journal
        if (m_api_gateway)
        {
            //--- parse the parameters
            va_start(arg_ptr, format);
            //--- form the string
            _vsnwprintf_s(m_prebuf, _countof(m_prebuf) - 1, _TRUNCATE, format, arg_ptr);
            //--- end of the list of arguments
            va_end(arg_ptr);
            //--- record to the journal
            m_api_gateway->LoggerOut(code, m_prebuf);
        }
        //--- unlock
        m_sync.Unlock();
    }
};
extern CLogger  ExtLogger;
//+------------------------------------------------------------------+
