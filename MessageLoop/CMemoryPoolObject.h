#pragma once

template<typename T>
T GetObject(MSG &msg)
{
    return (*(T*)(msg.lParam));
}
template<typename T>
T GetObject(LPVOID ptr)
{
    return (*(T*)(ptr));
}

template <typename T, unsigned MemoryPoolSize> class MemoryPool
{
private:
    T    _buf[MemoryPoolSize];
    BOOL _use[MemoryPoolSize];
    //UINT count;
public:
    MemoryPool()
    {
        ZeroMemory(_buf, sizeof(_buf));
        ZeroMemory(_use, sizeof(_use));
        //count = 0;
    };
    virtual ~MemoryPool(){};
    T* Allocate()
    {
        for (int i = 0; i < MemoryPoolSize; i++)
            if (!_use[i])
            {
                _use[i] = TRUE;
                //count++;
                //if (count > (MemoryPoolSize/2))
                //    ExtLogger.Out(MTLogWarn, L"%S[ %d / %d used ]", typeid(T).name(), count,MemoryPoolSize);
                return &_buf[i];
            }
        return NULL;
    }
    void Free(T* addr)
    {
        if (addr >= _buf)
        {
            if (_use[((UINT64)addr - (UINT64)_buf) / sizeof(T)] == TRUE)
            {
                _use[((UINT64)addr - (UINT64)_buf) / sizeof(T)] = FALSE;
                //count--;
            }
        }
    }
};
template <typename T, unsigned MemoryPoolSize=100> struct CMemoryPoolObject
{
public:
    static T* Allocate();
    static void Free(T* ptr);
    static void Free(LPARAM ptr){ Free((T*)ptr); };
    static void Free(PVOID ptr){ Free((T*)ptr); };
    bool post(UINT msg_type, IMessageListener* msg_listener);
private:
    static MemoryPool<T, MemoryPoolSize>& GetPool();
};

template <typename T, unsigned MemoryPoolSize = 100>
T* CMemoryPoolObject<T, MemoryPoolSize>::Allocate()
{
    return GetPool().Allocate();
}
template <typename T, unsigned MemoryPoolSize = 100>
void CMemoryPoolObject<T, MemoryPoolSize>::Free(T* ptr)
{
    GetPool().Free(ptr);
}
template <typename T, unsigned MemoryPoolSize>
MemoryPool<T,MemoryPoolSize>& CMemoryPoolObject<T,MemoryPoolSize>::GetPool()
{
    static MemoryPool<T,MemoryPoolSize> pool;
    return pool;
}
template <typename T, unsigned MemoryPoolSize = 100>
bool CMemoryPoolObject<T, MemoryPoolSize>::post(UINT msg_type, IMessageListener* msg_listener)
{
    T *obj = T::Allocate();
    if (obj)
    {
        CopyMemory(obj, this, sizeof(T));
        if (msg_listener->msg(msg_type, 0, (LPARAM)obj) == 0)
        {
            ExtLogger.Out(MTLogErr, L"failed to post thread message for %d",msg_type);
            return false;
        }
        else
            return true;
    }
    else
    {
        ExtLogger.Out(MTLogErr, L"failed to allocate memory %S %d", typeid(T).name(),MemoryPoolSize);
        return false;
    }
}