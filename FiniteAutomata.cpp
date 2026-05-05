#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <string>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <sstream>

using namespace std;

// ==================== Data Structures ====================
struct NFA
{
    int numStates = 0;
    set<int> startStates;
    set<int> finalStates;
    set<char> alphabet;
    map<int, map<char, set<int>>> transitions;
    map<int, set<int>> epsilonTransitions; // ε-transitions support
};

struct DFA
{
    int numStates = 0;
    int startState = -1;
    set<int> finalStates;
    set<char> alphabet;
    map<int, map<char, int>> transitions;
    map<int, set<int>> stateToSubset; // Maps DFA state ID → NFA subset
};

// ==================== Helper Functions ====================
void clearInputBuffer()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void waitForUser()
{
    cout << "\n[Press Enter to continue...]";
    cin.get();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string trim(const string &s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

string formatStateSubset(const DFA &dfa, int stateId)
{
    if (!dfa.stateToSubset.count(stateId))
        return "q" + to_string(stateId);
    string result = "{";
    bool first = true;
    for (int s : dfa.stateToSubset.at(stateId))
    {
        if (!first) result += ",";
        result += "q" + to_string(s);
        first = false;
    }
    result += "}";
    return result;
}

// ==================== ε-Closure ====================
set<int> getEpsilonClosure(const NFA &nfa, const set<int> &states)
{
    set<int> closure = states;
    queue<int> q;
    for (int s : states)
        q.push(s);

    while (!q.empty())
    {
        int curr = q.front();
        q.pop();
        if (nfa.epsilonTransitions.count(curr))
        {
            for (int next : nfa.epsilonTransitions.at(curr))
            {
                if (closure.insert(next).second)
                {
                    q.push(next);
                }
            }
        }
    }
    return closure;
}

// ==================== Visualization ====================
void drawNFAGraphical(const NFA &nfa)
{
    cout << "\n+==================================================+\n";
    cout << "|                 NFA GRAPHICAL VIEW                |\n";
    cout << "+==================================================+\n";

    cout << "    ";
    for (int i = 0; i < nfa.numStates; i++)
    {
        if (nfa.startStates.count(i))
            cout << "->";
        cout << (nfa.finalStates.count(i) ? "[*]" : "[ ]") << "q" << i;
        if (i < nfa.numStates - 1)
            cout << "   ";
    }
    cout << "\n\nAlphabet: {";
    bool first = true;
    for (char c : nfa.alphabet)
    {
        cout << (first ? "" : ", ") << c;
        first = false;
    }
    cout << "}\n";

    cout << "\nTRANSITIONS:\n"
         << string(50, '-') << "\n";
    for (int i = 0; i < nfa.numStates; i++)
    {
        for (char sym : nfa.alphabet)
        {
            if (nfa.transitions.count(i) && nfa.transitions.at(i).count(sym))
            {
                for (int target : nfa.transitions.at(i).at(sym))
                {
                    cout << "  q" << i << " --" << sym << "--> q" << target << "\n";
                }
            }
        }
        if (nfa.epsilonTransitions.count(i))
        {
            for (int target : nfa.epsilonTransitions.at(i))
            {
                cout << "  q" << i << " --ε--> q" << target << "\n";
            }
        }
    }

    cout << "\n"
         << string(60, '=') << "\n";
    cout << "TRANSITION TABLE:\n"
         << string(60, '=') << "\n";

    vector<vector<string>> matrix(nfa.numStates, vector<string>(nfa.numStates, "-"));
    for (int i = 0; i < nfa.numStates; i++)
    {
        for (char sym : nfa.alphabet)
        {
            if (nfa.transitions.count(i) && nfa.transitions.at(i).count(sym))
            {
                for (int target : nfa.transitions.at(i).at(sym))
                {
                    if (matrix[i][target] == "-")
                        matrix[i][target] = string(1, sym);
                    else
                        matrix[i][target] += "," + string(1, sym);
                }
            }
        }
    }

    cout << "      ";
    for (int j = 0; j < nfa.numStates; j++)
        cout << " q" << j << "  ";
    cout << "\n"
         << string(5 + nfa.numStates * 5, '-') << "\n";

    for (int i = 0; i < nfa.numStates; i++)
    {
        cout << " q" << i << " |";
        for (int j = 0; j < nfa.numStates; j++)
        {
            cout << " " << (matrix[i][j] == "-" ? "-" : matrix[i][j]) << "  ";
        }
        cout << "\n";
    }
}

void drawDFAGraphical(const DFA &dfa)
{
    cout << "\n+==================================================+\n";
    cout << "|                 DFA GRAPHICAL VIEW                |\n";
    cout << "+==================================================+\n";

    cout << "    ";
    for (int i = 0; i < dfa.numStates; i++)
    {
        if (i == dfa.startState)
            cout << "->";
        cout << (dfa.finalStates.count(i) ? "[*]" : "[ ]") << formatStateSubset(dfa, i);
        if (i < dfa.numStates - 1)
            cout << "   ";
    }
    cout << "\n\nAlphabet: {";
    bool first = true;
    for (char c : dfa.alphabet)
    {
        cout << (first ? "" : ", ") << c;
        first = false;
    }
    cout << "}\n";

    cout << "\n"
         << string(60, '=') << "\n";
    cout << "TRANSITION TABLE:\n"
         << string(60, '=') << "\n";

    // Calculate dynamic column width
    int maxWidth = 3;
    for (int i = 0; i < dfa.numStates; i++)
    {
        string rep = formatStateSubset(dfa, i);
        maxWidth = max(maxWidth, (int)rep.length());
        // Also check transition targets
        if (dfa.transitions.count(i))
        {
            for (char sym : dfa.alphabet)
            {
                if (dfa.transitions.at(i).count(sym))
                {
                    string target = formatStateSubset(dfa, dfa.transitions.at(i).at(sym));
                    maxWidth = max(maxWidth, (int)target.length());
                }
            }
        }
    }
    int colWidth = max(maxWidth + 4, 7);

    // Table header
    cout << "+" << string(colWidth, '-');
    for (size_t i = 0; i < dfa.alphabet.size(); i++)
        cout << "+" << string(colWidth, '-');
    cout << "+\n| " << left << setw(colWidth - 2) << "State";
    for (char sym : dfa.alphabet)
        cout << "| " << setw(colWidth - 2) << string(1, sym);
    cout << "|\n+" << string(colWidth, '-');
    for (size_t i = 0; i < dfa.alphabet.size(); i++)
        cout << "+" << string(colWidth, '-');
    cout << "+\n";

    // Table rows
    for (int i = 0; i < dfa.numStates; i++)
    {
        cout << "| " << left << setw(colWidth - 2) << formatStateSubset(dfa, i);
        for (char sym : dfa.alphabet)
        {
            if (dfa.transitions.count(i) && dfa.transitions.at(i).count(sym))
            {
                cout << "| " << setw(colWidth - 2) << formatStateSubset(dfa, dfa.transitions.at(i).at(sym));
            }
            else
            {
                cout << "| " << setw(colWidth - 2) << "-";
            }
        }
        cout << "|\n";
    }
    cout << "+" << string(colWidth, '-');
    for (size_t i = 0; i < dfa.alphabet.size(); i++)
        cout << "+" << string(colWidth, '-');
    cout << "+\n";

    cout << "\nVISUAL FLOW:\n"
         << string(50, '-') << "\n";
    for (int i = 0; i < dfa.numStates; i++)
    {
        for (char sym : dfa.alphabet)
        {
            if (dfa.transitions.count(i) && dfa.transitions.at(i).count(sym))
            {
                int t = dfa.transitions.at(i).at(sym);
                cout << "  " << formatStateSubset(dfa, i) << " --" << sym << "--> " << formatStateSubset(dfa, t);
                if (t == i)
                    cout << " (self-loop)";
                if (dfa.finalStates.count(t))
                    cout << " [FINAL]";
                cout << "\n";
            }
        }
    }
}

// ==================== Core Algorithms ====================
DFA nfaToDfa(const NFA &nfa)
{
    DFA dfa;
    dfa.alphabet = nfa.alphabet;
    map<set<int>, int> subsetMap;
    queue<set<int>> unprocessed;
    int nextId = 0;

    set<int> startClosure = getEpsilonClosure(nfa, nfa.startStates);
    subsetMap[startClosure] = nextId++;
    dfa.startState = 0;
    dfa.stateToSubset[0] = startClosure;
    unprocessed.push(startClosure);

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
            if (nfa.finalStates.count(s))
            {
                dfa.finalStates.insert(currId);
                break;
            }
        }

        for (char sym : dfa.alphabet)
        {
            set<int> moveSet;
            for (int state : current)
            {
                if (nfa.transitions.count(state) && nfa.transitions.at(state).count(sym))
                {
                    for (int t : nfa.transitions.at(state).at(sym))
                    {
                        moveSet.insert(t);
                    }
                }
            }
            set<int> nextClosure = getEpsilonClosure(nfa, moveSet);

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
                cout << " → No transition (empty set)";
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
        cout << (first ? "" : ", ") << "{";
        bool innerFirst = true;
        for (int s : pair.second)
        {
            cout << (innerFirst ? "" : ", ") << "q" << s;
            innerFirst = false;
        }
        cout << "}";
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
        for (int s : dfa.stateToSubset[dfaState])
        {
            cout << (innerFirst ? "" : ", ") << "q" << s;
            innerFirst = false;
        }
        cout << "}";
        first = false;
    }
    cout << " }\n";

    return dfa;
}

bool simulateDfa(const DFA &dfa, const string &input)
{
    int current = dfa.startState;
    cout << "\n[TRACE] Input: \"" << input << "\"\n";
    cout << "  Start: " << formatStateSubset(dfa, current) << "\n";

    for (char c : input)
    {
        if (dfa.transitions.count(current) && dfa.transitions.at(current).count(c))
        {
            current = dfa.transitions.at(current).at(c);
            cout << "  --'" << c << "'--> " << formatStateSubset(dfa, current) << "\n";
        }
        else
        {
            cout << "  --'" << c << "'--> REJECTED (no transition)\n";
            return false;
        }
    }

    bool accepted = dfa.finalStates.count(current) > 0;
    cout << "  Final: " << formatStateSubset(dfa, current) << " | Result: "
         << (accepted ? "✓ ACCEPTED" : "✗ REJECTED") << "\n";
    return accepted;
}

// ==================== User Input ====================
void showInputGuide()
{
    cout << "\n[INPUT GUIDE] Create Your NFA:\n";
    cout << "1. States are numbered 0 to n-1\n";
    cout << "2. Enter start/final states as space-separated IDs\n";
    cout << "3. Alphabet: single characters (e.g., a b)\n";
    cout << "4. Transitions format: FROM SYMBOL TO\n";
    cout << "5. Use $ for ε-transitions (e.g., 0 $ 1)\n";
}

NFA inputCustomNFA()
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

        if (from >= 0 && from < nfa.numStates &&
            to >= 0 && to < nfa.numStates)
        {
            if (sym == '$')
            {
                nfa.epsilonTransitions[from].insert(to);
            }
            else if (nfa.alphabet.count(sym))
            {
                nfa.transitions[from][sym].insert(to);
            }
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

// ==================== Main Program ====================
int main()
{
    cout << "\n+==================================================+\n";
    cout << "|   NFA → DFA CONVERTER & SIMULATOR               |\n";
    cout << "|   Theory of Computation - Final Project 2026    |\n";
    cout << "+==================================================+\n";

    NFA currentNFA;
    DFA currentDFA;
    int choice;

    do
    {
        cout << "\n+----------------- MAIN MENU -----------------+\n";
        cout << "| 1. Input NFA → DFA & Simulate              |\n";
        cout << "| 0. Exit                                     |\n";
        cout << "+---------------------------------------------+\n";
        cout << "Choice: ";
        if (!(cin >> choice) || choice < 0 || choice > 1)
        {
            choice = -1;
            cout << "[ERROR] Invalid Input choose from 0-1.\n";
            clearInputBuffer();
            continue;
        }

        switch (choice)
        {
        case 1:
            if (currentNFA.numStates != 0)
            {
                cout << "\n[INFO] Previous NFA/DFA will be cleared.\n";
            }
            currentNFA = inputCustomNFA();
            cout << "\n[AUTO] Converting NFA → DFA...\n";
            currentDFA = nfaToDfa(currentNFA);

            drawNFAGraphical(currentNFA);
            waitForUser();

            drawDFAGraphical(currentDFA);
            waitForUser();

            cout << "\n[Test Strings] Type 'exit' to stop:\n";
            while (true)
            {
                cout << "> ";
                string input;
                getline(cin >> ws, input);
                input = trim(input);
                if (input.empty())
                    continue;
                if (input == "exit")
                    break;

                bool valid = true;
                for (char c : input)
                {
                    if (!currentDFA.alphabet.count(c))
                    {
                        valid = false;
                        break;
                    }
                }
                if (!valid)
                {
                    cout << "[WARN] Invalid symbols.\n";
                    continue;
                }

                simulateDfa(currentDFA, input);
            }
            break;

        case 0:
            cout << "\n[INFO] Good luck with your TOC project! 🎓\n";
            break;

        default:
            cout << "[ERROR] Invalid Input choose from 0-1.\n";
            waitForUser();
        }
    } while (choice != 0);

    return 0;
}
