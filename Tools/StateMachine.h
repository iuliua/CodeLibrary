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
                doAction(transition.action);
                m_current_state = transition.next;
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