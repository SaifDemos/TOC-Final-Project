#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <string>
#include <algorithm>
#include <limits>

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
    clearInputBuffer();
    cin.get();
}

string trim(const string &s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

// Compute ε-closure of a set of states (essential for correct conversion)
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

// ==================== ASCII Visualization (Your Style, Enhanced) ====================
void drawNFAGraphical(const NFA &nfa)
{
    cout << "\n+==================================================+\n";
    cout << "|                 NFA GRAPHICAL VIEW                |\n";
    cout << "+==================================================+\n";

    // Draw states line
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

    // Draw transitions
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
        // Show ε-transitions if any
        if (nfa.epsilonTransitions.count(i))
        {
            for (int target : nfa.epsilonTransitions.at(i))
            {
                cout << "  q" << i << " --ε--> q" << target << "\n";
            }
        }
    }

    // Transition table
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

    // Draw states line
    cout << "    ";
    for (int i = 0; i < dfa.numStates; i++)
    {
        if (i == dfa.startState)
            cout << "->";
        cout << (dfa.finalStates.count(i) ? "[*]" : "[ ]") << "q" << i;
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

    // Transition table
    cout << "\n"
         << string(60, '=') << "\n";
    cout << "TRANSITION TABLE:\n"
         << string(60, '=') << "\n";

    cout << "+---------";
    for (size_t i = 0; i < dfa.alphabet.size(); i++)
        cout << "+---------";
    cout << "+\n| State   ";
    for (char sym : dfa.alphabet)
        cout << "|   " << sym << "     ";
    cout << "|\n+---------";
    for (size_t i = 0; i < dfa.alphabet.size(); i++)
        cout << "+---------";
    cout << "+\n";

    for (int i = 0; i < dfa.numStates; i++)
    {
        cout << "| q" << i << "     ";
        for (char sym : dfa.alphabet)
        {
            if (dfa.transitions.count(i) && dfa.transitions.at(i).count(sym))
            {
                cout << "|   q" << dfa.transitions.at(i).at(sym) << "   ";
            }
            else
            {
                cout << "|    -    ";
            }
        }
        cout << "|\n";
    }
    cout << "+---------";
    for (size_t i = 0; i < dfa.alphabet.size(); i++)
        cout << "+---------";
    cout << "+\n";

    // Visual flow
    cout << "\nVISUAL FLOW:\n"
         << string(50, '-') << "\n";
    for (int i = 0; i < dfa.numStates; i++)
    {
        for (char sym : dfa.alphabet)
        {
            if (dfa.transitions.count(i) && dfa.transitions.at(i).count(sym))
            {
                int t = dfa.transitions.at(i).at(sym);
                cout << "  q" << i << " --" << sym << "--> q" << t;
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

// NFA → DFA with ε-closure + step-by-step academic logging
DFA nfaToDfa(const NFA &nfa)
{
    DFA dfa;
    dfa.alphabet = nfa.alphabet;
    map<set<int>, int> subsetMap;
    queue<set<int>> unprocessed;
    int nextId = 0;

    // Start state = ε-closure of NFA start states
    set<int> startClosure = getEpsilonClosure(nfa, nfa.startStates);
    subsetMap[startClosure] = nextId++;
    dfa.startState = 0;
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

        // Mark final if contains any NFA final state
        for (int s : current)
        {
            if (nfa.finalStates.count(s))
            {
                dfa.finalStates.insert(currId);
                break;
            }
        }

        // Process each symbol
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

            if (!nextClosure.empty())
            {
                if (!subsetMap.count(nextClosure))
                {
                    subsetMap[nextClosure] = nextId++;
                    unprocessed.push(nextClosure);
                    cout << "[NEW] DFA state " << subsetMap[nextClosure]
                         << " = ε-closure(move({";
                    first = true;
                    for (int s : current)
                    {
                        cout << (first ? "" : ",") << s;
                        first = false;
                    }
                    cout << "}, '" << sym << "')) = {";
                    first = true;
                    for (int s : nextClosure)
                    {
                        cout << (first ? "" : ",") << s;
                        first = false;
                    }
                    cout << "}\n";
                }
                dfa.transitions[currId][sym] = subsetMap[nextClosure];
            }
        }
    }

    dfa.numStates = nextId;
    cout << "[SUCCESS] DFA created with " << dfa.numStates << " states.\n";
    return dfa;
}

// DFA → NFA (Reverse Conversion)
NFA dfaToNfa(const DFA &dfa)
{
    NFA nfa;
    nfa.numStates = dfa.numStates;
    nfa.startStates.insert(dfa.startState);
    nfa.finalStates = dfa.finalStates;
    nfa.alphabet = dfa.alphabet;

    for (int i = 0; i < dfa.numStates; i++)
    {
        if (dfa.transitions.count(i))
        {
            for (auto &[sym, target] : dfa.transitions.at(i))
            {
                nfa.transitions[i][sym].insert(target);
            }
        }
    }
    return nfa;
}

// DFA Simulation with step-by-step trace (for testing abb, aabb, ab)
bool simulateDfa(const DFA &dfa, const string &input)
{
    int current = dfa.startState;
    cout << "\n[TRACE] Input: \"" << input << "\"\n";
    cout << "  Start: q" << current << "\n";

    for (char c : input)
    {
        if (dfa.transitions.count(current) && dfa.transitions.at(current).count(c))
        {
            current = dfa.transitions.at(current).at(c);
            cout << "  --'" << c << "'--> q" << current << "\n";
        }
        else
        {
            cout << "  --'" << c << "'--> REJECTED (no transition)\n";
            return false;
        }
    }

    bool accepted = dfa.finalStates.count(current) > 0;
    cout << "  Final: q" << current << " | Result: "
         << (accepted ? "✓ ACCEPTED" : "✗ REJECTED") << "\n";
    return accepted;
}

// ==================== Predefined Examples (From Your Code) ====================
NFA getExample_Abb()
{ // Strings ending with "ab"
    NFA nfa;
    nfa.numStates = 3;
    nfa.startStates = {0};
    nfa.finalStates = {2};
    nfa.alphabet = {'a', 'b'};
    nfa.transitions[0]['a'] = {0, 1};
    nfa.transitions[0]['b'] = {0};
    nfa.transitions[1]['b'] = {2};
    return nfa;
}

NFA getExample_AaBb()
{ // Strings containing "aa" or "bb"
    NFA nfa;
    nfa.numStates = 5;
    nfa.startStates = {0};
    nfa.finalStates = {3, 4};
    nfa.alphabet = {'a', 'b'};
    nfa.transitions[0]['a'] = {0, 1};
    nfa.transitions[0]['b'] = {0, 2};
    nfa.transitions[1]['a'] = {3};
    nfa.transitions[2]['b'] = {4};
    return nfa;
}

// ==================== User Input with Auto-Convert ====================
void showInputGuide()
{
    cout << "\n[INPUT GUIDE] Create Your NFA:\n";
    cout << "1. States are numbered 0 to n-1\n";
    cout << "2. Enter start/final states as space-separated IDs\n";
    cout << "3. Alphabet: single characters (e.g., a b)\n";
    cout << "4. Transitions format: FROM SYMBOL TO\n";
    cout << "5. Use $ for ε-transitions (e.g., 0 $ 1)\n";
    cout << "\nExample for (a|b)*abb:\n";
    cout << "  States: 4 | Start: 0 | Final: 3\n";
    cout << "  Alphabet: a b\n";
    cout << "  Transitions:\n";
    cout << "    0 a 0   0 a 1   0 b 0   1 b 2   2 b 3\n";
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
    cout << "|   NFA ↔ DFA CONVERTER & SIMULATOR               |\n";
    cout << "|   Theory of Computation - Final Project 2026    |\n";
    cout << "+==================================================+\n";

    NFA currentNFA;
    DFA currentDFA;
    int choice;

    do
    {
        cout << "\n+----------------- MAIN MENU -----------------+\n";
        cout << "| 1. Load Example: Strings ending with 'ab'   |\n";
        cout << "| 2. Load Example: Strings with 'aa' or 'bb'  |\n";
        cout << "| 3. Input Custom NFA (AUTO-CONVERT to DFA)   |\n";
        cout << "| 4. Convert Current NFA → DFA                |\n";
        cout << "| 5. Convert Current DFA → NFA (Reverse)      |\n";
        cout << "| 6. Simulate DFA: Test abb, aabb, ab, etc.   |\n";
        cout << "| 7. Visualize Current NFA                    |\n";
        cout << "| 8. Visualize Current DFA                    |\n";
        cout << "| 0. Exit                                     |\n";
        cout << "+---------------------------------------------+\n";
        cout << "Choice: ";
        if (!(cin >> choice) || choice < 0 || choice > 8)
        {
            choice = -1;
            cout << "[ERROR] Invalid Input choose from 0-8.\n";
            clearInputBuffer();
            continue;
        }

        switch (choice)
        {
        case 1:
            currentNFA = getExample_Abb();
            cout << "[OK] Loaded: Strings ending with 'ab'\n";
            drawNFAGraphical(currentNFA);
            // AUTO-CONVERT
            cout << "\n[AUTO] Converting NFA → DFA...\n";
            currentDFA = nfaToDfa(currentNFA);
            drawDFAGraphical(currentDFA);
            waitForUser();
            break;

        case 2:
            currentNFA = getExample_AaBb();
            cout << "[OK] Loaded: Strings containing 'aa' or 'bb'\n";
            drawNFAGraphical(currentNFA);
            // AUTO-CONVERT
            cout << "\n[AUTO] Converting NFA → DFA...\n";
            currentDFA = nfaToDfa(currentNFA);
            drawDFAGraphical(currentDFA);
            waitForUser();
            break;

        case 3:
            currentNFA = inputCustomNFA();
            cout << "\n[OK] Custom NFA loaded!\n";
            drawNFAGraphical(currentNFA);
            // AUTO-CONVERT TO DFA
            cout << "\n[AUTO] Converting your NFA → DFA...\n";
            currentDFA = nfaToDfa(currentNFA);
            drawDFAGraphical(currentDFA);
            waitForUser();
            break;

        case 4:
            if (currentNFA.numStates == 0)
            {
                cout << "[WARN] Load an NFA first.\n";
                break;
            }
            currentDFA = nfaToDfa(currentNFA);
            drawDFAGraphical(currentDFA);
            waitForUser();
            break;

        case 5:
            if (currentDFA.numStates == 0)
            {
                cout << "[WARN] Convert NFA→DFA first.\n";
                break;
            }
            currentNFA = dfaToNfa(currentDFA);
            cout << "[OK] DFA → NFA conversion complete.\n";
            drawNFAGraphical(currentNFA);
            waitForUser();
            break;

        case 6: // SIMULATE DFA ON TEST STRINGS
            if (currentDFA.numStates == 0)
            {
                cout << "[WARN] No DFA available.\n";
                break;
            }
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

                // Validate alphabet
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

                simulateDfa(currentDFA, input); // Shows ACCEPT/REJECT
            }
            break;

        case 7:
            if (currentNFA.numStates == 0)
                cout << "[WARN] No NFA.\n";
            else
                drawNFAGraphical(currentNFA);
            waitForUser();
            break;

        case 8:
            if (currentDFA.numStates == 0)
                cout << "[WARN] No DFA.\n";
            else
                drawDFAGraphical(currentDFA);
            waitForUser();
            break;

        case 0:
            cout << "\n[INFO] Good luck with your TOC project! 🎓\n";
            break;

        default:
            cout << "[ERROR] Invalid Input choose from 0-8.\n";
            waitForUser();
        }
    } while (choice != 0);

    return 0;
}