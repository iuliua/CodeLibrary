#pragma once
#include <vector>
namespace Tools
{
    class CStateMachine
    {
    public:
        CStateMachine() :m_current_state(-1)
            , hThread(INVALID_HANDLE_VALUE)
        {
            DWORD thread_id;
            hThread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcWrapper, this, 0, &thread_id);
        };

        virtual void doAction(int id) = 0;
        void event(int event)
        {
            CSync lock(m_cs);
            qDataQueue.push(event);
            
        }
        bool event_available()
        {
            CSync lock(m_cs);
            UINT size=qDataQueue.size();
            return size > 0;
        }
        ~CStateMachine()
        {
            CloseHandle(hThread);
        }
    private:
        std::queue<int> qDataQueue;
        CSync::CriticalSection m_cs;
        HANDLE hThread;
    protected:

        static DWORD ThreadProcWrapper(LPVOID arg)
        {
            ((CStateMachine*)arg)->ThreadProc();
            return 0;
        }
        void ThreadProc()
        {
            while (true)
            { 
                size_t queue_size=0;
                int event = 0;
                {
                    CSync sync(m_cs);
                    queue_size = qDataQueue.size();
                    event = qDataQueue.front();
                    qDataQueue.pop();
                }
                if (queue_size>0)
                {
                    for (auto &transition : m_transitions)
                    {
                        if (transition.current != m_current_state || transition.event != event)
                            continue; 
                        m_current_state = transition.next;
                        doAction(transition.action);
                        break;
                    }
                }
                else
                    Sleep(100);
            }
        }
        struct Transition
        {
            int current;
            int event;
            int next;
            int action;
            Transition(int a, int b, int c, int d) :current(a), event(b), next(c), action(d){};
        };
        int m_current_state;
        std::vector<Transition> m_transitions;
    };
}
/*
class COneLedMachine : public CStateMachine
{
public:
    enum Event { PowerOn, PowerOff, LedOn, LedOff, evSleep };
    enum Action { Action1, Action2, Action3, Action4, Action5, Action6, Action7, ActionSleep };
    enum State { STATE_POWER_OFF, STATE_POWER_ON, STATE_LED_ON, STATE_LED_OFF };
private:
    void PrintLine(LPCSTR s)
    {
        std::cout << s << std::endl;
    }
public:
    COneLedMachine()
    {
        m_transitions.push_back(Transition(STATE_POWER_OFF, PowerOn, STATE_POWER_ON, Action1));
        m_transitions.push_back(Transition(STATE_POWER_ON, PowerOff, STATE_POWER_OFF, Action2));
        m_transitions.push_back(Transition(STATE_POWER_ON, evSleep, STATE_POWER_ON, ActionSleep));
        m_transitions.push_back(Transition(STATE_POWER_ON, LedOn, STATE_LED_ON, Action3));
        m_transitions.push_back(Transition(STATE_LED_ON, PowerOff, STATE_POWER_OFF, Action4));
        m_transitions.push_back(Transition(STATE_LED_ON, LedOff, STATE_LED_OFF, Action5));
        m_transitions.push_back(Transition(STATE_LED_OFF, LedOn, STATE_LED_ON, Action3));
        m_transitions.push_back(Transition(STATE_LED_OFF, PowerOff, STATE_POWER_OFF, Action7));
        m_current_state = STATE_POWER_OFF;
    }
    virtual void doAction(int id) override
    {
        switch (id)
        {
        case Action1:
            PrintLine("powering on");
            break;
        case Action2:
            PrintLine("powering off");
            break;
        case Action3:
            PrintLine("led on");
            break;
        case Action4:
            PrintLine("powering off while led is on");
            break;
        case Action5:
            PrintLine("led off");
            break;
        case Action6:
            PrintLine("powering on");
            break;
        case Action7:
            PrintLine("powering off while led is off");
            break;
        case ActionSleep:
            PrintLine("sleeping");
            Sleep(3000);
            break;
        }
    }
};
*/