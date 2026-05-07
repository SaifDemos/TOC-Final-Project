#include "../Headers/NFA.h"

using namespace std;

set<int> NFA::getEpsilonClosure(const set<int> &states) const
{
    set<int> closure = states;
    queue<int> q;
    for (int s : states)
        q.push(s);

    while (!q.empty())
    {
        int curr = q.front();
        q.pop();
        auto it = epsilonTransitions.find(curr);
        if (it != epsilonTransitions.end())
        {
            for (int next : it->second)
            {
                if (closure.insert(next).second)
                    q.push(next);
            }
        }
    }
    return closure;
}

set<int> NFA::getTransition(const set<int> &states, char sym) const
{
    set<int> result;
    for (int s : states)
    {
        auto it = transitions.find(s);
        if (it != transitions.end())
        {
            auto sit = it->second.find(sym);
            if (sit != it->second.end())
            {
                for (int t : sit->second)
                    result.insert(t);
            }
        }
    }
    return result;
}

bool NFA::isFinal(int state) const
{
    return finalStates.count(state) > 0;
}
