#include "../Headers/UserInputHandler.h"
#include <iostream>

using namespace std;

void UserInputHandler::showInputGuide()
{
    cout << "\n[INPUT GUIDE] Create Your NFA:\n";
    cout << "1. States are numbered 0 to n-1\n";
    cout << "2. Enter start/final states as space-separated IDs\n";
    cout << "3. Alphabet: single characters (e.g., a b)\n";
    cout << "4. Transitions format: FROM SYMBOL TO\n";
    cout << "5. Use $ for ε-transitions (e.g., 0 $ 1)\n";
}

NFA UserInputHandler::inputCustomNFA()
{
    NFA nfa;
    showInputGuide();

    cout << "\n--- Enter Your NFA ---\n";
    cout << "Number of states: ";
    cin >> nfa.numStates;

    int startCount;
    cout << "Number of start states: ";
    cin >> startCount;
    cout << "Start state IDs: ";
    for (int i = 0; i < startCount; i++)
    {
        int s;
        cin >> s;
        if (s >= 0 && s < nfa.numStates)
            nfa.startStates.insert(s);
        else
        {
            cout << "  Invalid! ";
            i--;
        }
    }

    int finalCount;
    cout << "Number of final states: ";
    cin >> finalCount;
    cout << "Final state IDs: ";
    for (int i = 0; i < finalCount; i++)
    {
        int f;
        cin >> f;
        if (f >= 0 && f < nfa.numStates)
            nfa.finalStates.insert(f);
        else
        {
            cout << "  Invalid! ";
            i--;
        }
    }

    int alphaSize;
    cout << "Alphabet size: ";
    cin >> alphaSize;
    cout << "Alphabet characters: ";
    for (int i = 0; i < alphaSize; i++)
    {
        char c;
        cin >> c;
        nfa.alphabet.insert(c);
    }

    int transCount;
    cout << "Number of transitions: ";
    cin >> transCount;
    cout << "Enter transitions (FROM SYMBOL TO):\n";
    for (int i = 0; i < transCount; i++)
    {
        int from, to;
        char sym;
        cout << "  #" << (i + 1) << ": ";
        cin >> from >> sym >> to;

        if (from >= 0 && from < nfa.numStates && to >= 0 && to < nfa.numStates)
        {
            if (sym == '$')
                nfa.epsilonTransitions[from].insert(to);
            else if (nfa.alphabet.count(sym))
                nfa.transitions[from][sym].insert(to);
            else
            {
                cout << "  Invalid! Try again.\n";
                i--;
            }
        }
        else
        {
            cout << "  Invalid! Try again.\n";
            i--;
        }
    }
    return nfa;
}

DFA UserInputHandler::inputCustomDFA()
{
    DFA dfa;
    cout << "\n--- Enter Your DFA ---\n";
    cout << "Number of states: ";
    cin >> dfa.numStates;

    cout << "Start state ID: ";
    cin >> dfa.startState;
    if (dfa.startState < 0 || dfa.startState >= dfa.numStates)
    {
        cout << "[ERROR] Invalid start state. Setting to 0.\n";
        dfa.startState = 0;
    }

    int finalCount;
    cout << "Number of final states: ";
    cin >> finalCount;
    cout << "Final state IDs: ";
    for (int i = 0; i < finalCount; i++)
    {
        int f;
        cin >> f;
        if (f >= 0 && f < dfa.numStates)
            dfa.finalStates.insert(f);
        else
        {
            cout << "  Invalid! ";
            i--;
        }
    }

    int alphaSize;
    cout << "Alphabet size: ";
    cin >> alphaSize;
    cout << "Alphabet characters: ";
    for (int i = 0; i < alphaSize; i++)
    {
        char c;
        cin >> c;
        dfa.alphabet.insert(c);
    }

    int transCount;
    cout << "Number of transitions: ";
    cin >> transCount;
    cout << "Enter transitions (FROM SYMBOL TO):\n";
    for (int i = 0; i < transCount; i++)
    {
        int from, to;
        char sym;
        cout << "  #" << (i + 1) << ": ";
        cin >> from >> sym >> to;

        if (from >= 0 && from < dfa.numStates && to >= 0 && to < dfa.numStates && dfa.alphabet.count(sym))
            dfa.transitions[from][sym] = to;
        else
        {
            cout << "  Invalid! Try again.\n";
            i--;
        }
    }
    return dfa;
}
