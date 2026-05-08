#include "../Headers/UserInputHandler.h"
#include "../Headers/Utils.h"
#include <iostream>
#include <sstream>

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

    // Number of states
    while (true)
    {
        cout << "Number of states: ";
        if (readPositiveInt(nfa.numStates, "[ERROR] Enter a positive number of states\n"))
            break;
        if (cin.eof()) return nfa;
    }

    // Start states
    int startCount;
    while (true)
    {
        cout << "Number of start states: ";
        if (readInt(startCount, 0, nfa.numStates, "[ERROR] Enter 0 to " + to_string(nfa.numStates) + "\n"))
            break;
        if (cin.eof()) return nfa;
    }

    cout << "Start state IDs: ";
    int entered = 0;
    while (entered < startCount)
    {
        if (cin.eof()) return nfa;
        int s;
        if (!readInt(s, 0, nfa.numStates - 1, "  Invalid! "))
            continue;
        nfa.startStates.insert(s);
        entered++;
    }

    // Final states
    int finalCount;
    while (true)
    {
        cout << "Number of final states: ";
        if (readInt(finalCount, 0, nfa.numStates, "[ERROR] Enter 0 to " + to_string(nfa.numStates) + "\n"))
            break;
        if (cin.eof()) return nfa;
    }

    cout << "Final state IDs: ";
    entered = 0;
    while (entered < finalCount)
    {
        if (cin.eof()) return nfa;
        int f;
        if (!readInt(f, 0, nfa.numStates - 1, "  Invalid! "))
            continue;
        nfa.finalStates.insert(f);
        entered++;
    }

    // Alphabet
    int alphaSize;
    while (true)
    {
        cout << "Alphabet size: ";
        if (readPositiveInt(alphaSize, "[ERROR] Enter a positive number\n"))
            break;
        if (cin.eof()) return nfa;
    }

    while (true)
    {
        cout << "Alphabet characters (" << alphaSize << " space-separated): ";
        set<char> tempAlpha;
        int count = 0;
        while (count < alphaSize)
        {
            if (cin.eof()) return nfa;
            char c;
            if (!readChar(c, "  Invalid! "))
                continue;
            tempAlpha.insert(c);
            count++;
        }
        // Consume rest of line to prevent overflow
        if (cin.rdstate() & ios::failbit) cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (tempAlpha.size() != (size_t)alphaSize)
        {
            cout << "  [ERROR] Expected " << alphaSize << " unique characters, got " << tempAlpha.size() << ". Try again.\n";
            tempAlpha.clear();
            continue;
        }
        nfa.alphabet = tempAlpha;
        break;
    }
    
    // Transitions
    int transCount;
    int maxTransitions = nfa.numStates * nfa.numStates * (nfa.alphabet.size() + 1);
    while (true)
    {
        cout << "Number of transitions (max " << maxTransitions << "): ";
        if (readInt(transCount, 1, maxTransitions, "[ERROR] Enter 1 to " + to_string(maxTransitions) + "\n"))
            break;
        if (cin.eof()) return nfa;
    }

    cout << "Enter transitions (FROM SYMBOL TO):\n";
    entered = 0;
    while (entered < transCount)
    {
        if (cin.eof()) return nfa;
        int from, to;
        char sym;
        cout << "  #" << (entered + 1) << ": ";
        if (!readInt(from, 0, nfa.numStates - 1, "  Invalid! ") ||
            !readChar(sym, "  Invalid! ") ||
            !readInt(to, 0, nfa.numStates - 1, "  Invalid! "))
            continue;

        if (sym == '$')
            nfa.epsilonTransitions[from].insert(to);
        else if (nfa.alphabet.count(sym))
            nfa.transitions[from][sym].insert(to);
        else
        {
            cout << "  Invalid symbol! Try again.\n";
            continue;
        }
        entered++;
    }
    clearInputBuffer();
    return nfa;
}

DFA UserInputHandler::inputCustomDFA()
{
    DFA dfa;
    cout << "\n--- Enter Your DFA ---\n";

    // Number of states
    while (true)
    {
        cout << "Number of states: " << flush;
        if (readPositiveInt(dfa.numStates, "[ERROR] Enter a positive number of states\n"))
            break;
        if (cin.eof()) return dfa;
    }

    // Start state
    while (true)
    {
        cout << "Start state ID: ";
        if (readInt(dfa.startState, 0, dfa.numStates - 1, "[ERROR] Enter 0 to " + to_string(dfa.numStates - 1) + "\n"))
            break;
        if (cin.eof()) return dfa;
    }

    // Final states
    int finalCount;
    while (true)
    {
        cout << "Number of final states: ";
        if (readInt(finalCount, 0, dfa.numStates, "[ERROR] Enter 0 to " + to_string(dfa.numStates) + "\n"))
            break;
        if (cin.eof()) return dfa;
    }

    cout << "Final state IDs: ";
    int entered = 0;
    while (entered < finalCount)
    {
        if (cin.eof()) return dfa;
        int f;
        if (!readInt(f, 0, dfa.numStates - 1, "  Invalid! "))
            continue;
        dfa.finalStates.insert(f);
        entered++;
    }

    // Alphabet
    int alphaSize;
    while (true)
    {
        cout << "Alphabet size: ";
        if (readPositiveInt(alphaSize, "[ERROR] Enter a positive number\n"))
            break;
        if (cin.eof()) return dfa;
    }

    while (true)
    {
        cout << "Alphabet characters (" << alphaSize << " space-separated): ";
        set<char> tempAlpha;
        int count = 0;
        while (count < alphaSize)
        {
            if (cin.eof()) return dfa;
            char c;
            if (!readChar(c, "  Invalid! "))
                continue;
            tempAlpha.insert(c);
            count++;
        }
        // Consume rest of line to prevent overflow
        if (cin.rdstate() & ios::failbit) cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (tempAlpha.size() != (size_t)alphaSize)
        {
            cout << "  [ERROR] Expected " << alphaSize << " unique characters, got " << tempAlpha.size() << ". Try again.\n";
            tempAlpha.clear();
            continue;
        }
        dfa.alphabet = tempAlpha;
        break;
    }
    
    // Transitions
    int transCount;
    int maxTransitions = dfa.numStates * dfa.alphabet.size();
    while (true)
    {
        cout << "Number of transitions (max " << maxTransitions << "): ";
        if (readInt(transCount, 1, maxTransitions, "[ERROR] Enter 1 to " + to_string(maxTransitions) + "\n"))
            break;
        if (cin.eof()) return dfa;
    }

    cout << "Enter transitions (FROM SYMBOL TO):\n";
    entered = 0;
    while (entered < transCount)
    {
        if (cin.eof()) return dfa;
        int from, to;
        char sym;
        cout << "  #" << (entered + 1) << ": ";
        if (!readInt(from, 0, dfa.numStates - 1, "  Invalid! ") ||
            !readChar(sym, "  Invalid! ") ||
            !readInt(to, 0, dfa.numStates - 1, "  Invalid! "))
            continue;

        if (from >= 0 && from < dfa.numStates && to >= 0 && to < dfa.numStates && dfa.alphabet.count(sym))
        {
            dfa.transitions[from][sym] = to;
            entered++;
        }
        else
        {
            cout << "  Invalid! Try again.\n";
        }
    }
    clearInputBuffer();
    return dfa;
}
