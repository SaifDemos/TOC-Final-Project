#include "../Headers/AutomataSimulator.h"
#include <iostream>

using namespace std;

bool AutomataSimulator::simulateDfa(const DFA &dfa, const string &input)
{
    int current = dfa.startState;
    cout << "\n[TRACE] Input: \"" << input << "\"\n";
    cout << "  Start: " << dfa.formatStateSubset(current) << "\n";

    for (char c : input)
    {
        int next = dfa.getTransition(current, c);
        if (next != -1)
        {
            current = next;
            cout << "  --'" << c << "'--> " << dfa.formatStateSubset(current) << "\n";
        }
        else
        {
            cout << "  --'" << c << "'--> REJECTED (no transition)\n";
            return false;
        }
    }

    bool accepted = dfa.isFinal(current);
    cout << "  Final: " << dfa.formatStateSubset(current) << " | Result: "
         << (accepted ? "✓ ACCEPTED" : "✗ REJECTED") << "\n";
    return accepted;
}

bool AutomataSimulator::simulateNfa(const NFA &nfa, const string &input)
{
    set<int> currentStates = nfa.getEpsilonClosure(nfa.startStates);
    cout << "\n[TRACE] Input: \"" << input << "\"\n";
    cout << "  Start: {";
    bool first = true;
    for (int s : currentStates)
    {
        cout << (first ? "" : ", ") << "q" << s;
        first = false;
    }
    cout << "}\n";

    for (char c : input)
    {
        if (!nfa.alphabet.count(c))
        {
            cout << "  --'" << c << "'--> REJECTED (invalid symbol)\n";
            return false;
        }
        set<int> nextStates = nfa.getTransition(currentStates, c);
        nextStates = nfa.getEpsilonClosure(nextStates);

        cout << "  --'" << c << "'--> {";
        first = true;
        for (int s : nextStates)
        {
            cout << (first ? "" : ", ") << "q" << s;
            first = false;
        }
        cout << "}\n";

        if (nextStates.empty())
        {
            cout << "  REJECTED (no states)\n";
            return false;
        }
        currentStates = nextStates;
    }

    bool accepted = false;
    for (int s : currentStates)
    {
        if (nfa.isFinal(s))
        {
            accepted = true;
            break;
        }
    }
    cout << "  Final: {";
    first = true;
    for (int s : currentStates)
    {
        cout << (first ? "" : ", ") << "q" << s;
        first = false;
    }
    cout << "} | Result: " << (accepted ? "✓ ACCEPTED" : "✗ REJECTED") << "\n";
    return accepted;
}
