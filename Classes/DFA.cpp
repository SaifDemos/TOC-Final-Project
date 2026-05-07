#include "../Headers/DFA.h"
#include <algorithm>

using namespace std;

string DFA::formatStateSubset(int stateId) const
{
    auto it = stateToSubset.find(stateId);
    if (it == stateToSubset.end())
        return "q" + to_string(stateId);

    const set<int>& subset = it->second;
    if (subset.empty())
        return "φ";

    string result = "{";
    bool first = true;
    for (int s : subset)
    {
        if (!first) result += ",";
        result += "q" + to_string(s);
        first = false;
    }
    result += "}";
    return result;
}

int DFA::getTransition(int from, char sym) const
{
    auto it = transitions.find(from);
    if (it != transitions.end())
    {
        auto sit = it->second.find(sym);
        if (sit != it->second.end())
            return sit->second;
    }
    return -1;
}

bool DFA::isFinal(int state) const
{
    return finalStates.count(state) > 0;
}
