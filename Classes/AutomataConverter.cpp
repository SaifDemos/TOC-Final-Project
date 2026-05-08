#include "../Headers/AutomataConverter.h"
#include <iostream>
#include <queue>
#include <string>

using namespace std;

DFA AutomataConverter::nfaToDfa(const NFA &nfa)
{
    DFA dfa;
    dfa.alphabet = nfa.alphabet;
    map<set<int>, int> subsetMap;
    queue<set<int>> unprocessed;
    int nextId = 0;

    set<int> startClosure = nfa.getEpsilonClosure(nfa.startStates);
    subsetMap[startClosure] = nextId++;
    dfa.startState = 0;
    dfa.stateToSubset[0] = startClosure;
    unprocessed.push(startClosure);

    // Create dead state for empty set (φ)
    set<int> emptySet;
    subsetMap[emptySet] = nextId++;
    int deadStateId = subsetMap[emptySet];
    dfa.stateToSubset[deadStateId] = emptySet;
    for (char sym : dfa.alphabet)
        dfa.transitions[deadStateId][sym] = deadStateId;

    cout << "\n[CONVERSION STEPS]: NFA → DFA (Subset Construction)\n";
    cout << string(60, '-') << "\n";
    cout << "[STEP 0] Start = ε-closure({";
    bool first = true;
    for (int s : nfa.startStates)
    {
        cout << (first ? "" : ",") << s;
        first = false;
    }
    cout << "}) = {";
    first = true;
    for (int s : startClosure)
    {
        cout << (first ? "" : ",") << s;
        first = false;
    }
    cout << "} → DFA state 0\n";

    while (!unprocessed.empty())
    {
        set<int> current = unprocessed.front();
        unprocessed.pop();
        int currId = subsetMap[current];

        cout << "\n[PROCESSING] DFA state " << currId << " = {";
        first = true;
        for (int s : current)
        {
            cout << (first ? "" : ", ") << "q" << s;
            first = false;
        }
        cout << "}";

        for (int s : current)
        {
            if (nfa.isFinal(s))
            {
                dfa.finalStates.insert(currId);
                break;
            }
        }

        for (char sym : dfa.alphabet)
        {
            set<int> moveSet = nfa.getTransition(current, sym);
            set<int> nextClosure = nfa.getEpsilonClosure(moveSet);

            cout << "\n  δ'(" << currId << ", '" << sym << "') = ε-closure(move({";
            first = true;
            for (int s : current)
            {
                cout << (first ? "" : ", ") << "q" << s;
                first = false;
            }
            cout << "}, '" << sym << "')) = ε-closure({";
            first = true;
            for (int s : moveSet)
            {
                cout << (first ? "" : ", ") << "q" << s;
                first = false;
            }
            cout << "}) = {";
            first = true;
            for (int s : nextClosure)
            {
                cout << (first ? "" : ", ") << "q" << s;
                first = false;
            }
            cout << "}";

            if (!nextClosure.empty())
            {
                if (!subsetMap.count(nextClosure))
                {
                    subsetMap[nextClosure] = nextId++;
                    dfa.stateToSubset[subsetMap[nextClosure]] = nextClosure;
                    unprocessed.push(nextClosure);
                    cout << " → [NEW] DFA state " << subsetMap[nextClosure];
                }
                else
                {
                    cout << " → Existing DFA state " << subsetMap[nextClosure];
                }
                dfa.transitions[currId][sym] = subsetMap[nextClosure];
            }
            else
            {
                dfa.transitions[currId][sym] = deadStateId;
                cout << " → Dead state (φ)";
            }
        }
    }

    dfa.numStates = nextId;
    cout << "\n\n[SUCCESS] DFA created with " << dfa.numStates << " states.\n";

    cout << "\nFinal DFA Parameters:\n";
    cout << "Q' = { ";
    first = true;
    for (auto &pair : dfa.stateToSubset)
    {
        cout << (first ? "" : ", ") << dfa.formatStateSubset(pair.first);
        first = false;
    }
    cout << " }\n";

    cout << "Start state: {";
    first = true;
    for (int s : startClosure)
    {
        cout << (first ? "" : ", ") << "q" << s;
        first = false;
    }
    cout << "}\n";

    cout << "Final states: { ";
    first = true;
    for (int dfaState : dfa.finalStates)
    {
        cout << (first ? "" : ", ") << "{";
        bool innerFirst = true;
        auto it = dfa.stateToSubset.find(dfaState);
        if (it != dfa.stateToSubset.end())
        {
            for (int s : it->second)
            {
                cout << (innerFirst ? "" : ", ") << "q" << s;
                innerFirst = false;
            }
        }
        cout << "}";
        first = false;
    }
    cout << " }\n";

    return dfa;
}

NFA AutomataConverter::dfaToNfa(const DFA &dfa)
{
    NFA nfa;
    nfa.numStates = dfa.numStates;
    nfa.startStates.insert(dfa.startState);
    nfa.finalStates = dfa.finalStates;
    nfa.alphabet = dfa.alphabet;

    for (int from = 0; from < dfa.numStates; from++)
    {
        auto it = dfa.transitions.find(from);
        if (it != dfa.transitions.end())
        {
            for (char sym : dfa.alphabet)
            {
                auto sit = it->second.find(sym);
                if (sit != it->second.end())
                    nfa.transitions[from][sym].insert(sit->second);
            }
        }
    }
    return nfa;
}
