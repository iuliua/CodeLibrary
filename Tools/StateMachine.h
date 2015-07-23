#pragma once
#include <vector>
namespace Tools
{
    class CStateMachine
    {
    public:
        CStateMachine() :m_current_state(-1){};
        virtual void doAction(int id) = 0;
        void event(int event)
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
    protected:
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