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
#include <fstream>

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
    const set<int>& subset = dfa.stateToSubset.at(stateId);
    if (subset.empty()) {
        return "φ";
    }
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
    cout << "|                 NFA GRAPHICAL VIEW                 |\n";
    cout << "+====================================================+\n";

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

// ==================== Graphviz PDF Generation ====================
string generateNfaDot(const NFA &nfa)
{
    stringstream dot;
    dot << "digraph NFA {\n";
    dot << "  rankdir=LR;\n";
    dot << "  node [shape=circle, style=filled, fillcolor=\"#E8F4F8\"];\n";
    dot << "  edge [fontsize=10];\n\n";

    // Invisible start node
    dot << "  start [shape=plaintext, label=\"\"];\n\n";

    // Nodes
    for (int i = 0; i < nfa.numStates; i++)
    {
        dot << "  q" << i;
        if (nfa.finalStates.count(i))
            dot << " [shape=doublecircle]";
        dot << ";\n";
    }
    dot << "\n";

    // Start state arrows
    for (int s : nfa.startStates)
    {
        dot << "  start -> q" << s << ";\n";
    }
    dot << "\n";

    // Transitions
    for (int i = 0; i < nfa.numStates; i++)
    {
        for (char sym : nfa.alphabet)
        {
            if (nfa.transitions.count(i) && nfa.transitions.at(i).count(sym))
            {
                for (int target : nfa.transitions.at(i).at(sym))
                {
                    dot << "  q" << i << " -> q" << target << " [label=\"" << sym << "\"];\n";
                }
            }
        }
        if (nfa.epsilonTransitions.count(i))
        {
            for (int target : nfa.epsilonTransitions.at(i))
            {
                dot << "  q" << i << " -> q" << target << " [label=\"ε\", style=dashed];\n";
            }
        }
    }

    dot << "}\n";
    return dot.str();
}

string generateDfaDot(const DFA &dfa)
{
    stringstream dot;
    dot << "digraph DFA {\n";
    dot << "  rankdir=LR;\n";
    dot << "  node [shape=circle, style=filled, fillcolor=\"#E8F4F8\"];\n";
    dot << "  edge [fontsize=10];\n\n";

    // Invisible start node
    dot << "  start [shape=plaintext, label=\"\"];\n\n";

    // Nodes
    for (int i = 0; i < dfa.numStates; i++)
    {
        dot << "  state" << i;
        if (dfa.finalStates.count(i))
            dot << " [shape=doublecircle]";
        dot << " [label=\"" << formatStateSubset(dfa, i) << "\"]";
        dot << ";\n";
    }
    dot << "\n";

    // Start state arrow
    dot << "  start -> state" << dfa.startState << ";\n\n";

    // Transitions
    for (int i = 0; i < dfa.numStates; i++)
    {
        if (dfa.transitions.count(i))
        {
            for (char sym : dfa.alphabet)
            {
                if (dfa.transitions.at(i).count(sym))
                {
                    int target = dfa.transitions.at(i).at(sym);
                    dot << "  state" << i << " -> state" << target
                        << " [label=\"" << sym << "\"];\n";
                }
            }
        }
    }

    dot << "}\n";
    return dot.str();
}

bool saveDotToPdf(const string &dotCode, const string &filename)
{
    string dotFile = filename + ".dot";
    string pdfFile = filename + ".pdf";

    // Write DOT file
    ofstream out(dotFile);
    if (!out)
    {
        cout << "[ERROR] Cannot create " << dotFile << "\n";
        return false;
    }
    out << dotCode;
    out.close();

    // Run dot
    string cmd = "dot -Tpdf " + dotFile + " -o " + pdfFile;
    int result = system(cmd.c_str());

    // Clean up DOT file
    remove(dotFile.c_str());

    if (result != 0)
    {
        cout << "[ERROR] dot command failed. Is Graphviz installed?\n";
        return false;
    }

    cout << "[INFO] Saved to " << pdfFile << "\n";
    return true;
}

void saveNfaToPdf(const NFA &nfa, const string &baseName)
{
    string dot = generateNfaDot(nfa);
    saveDotToPdf(dot, baseName);
}

void saveDfaToPdf(const DFA &dfa, const string &baseName)
{
    string dot = generateDfaDot(dfa);
    saveDotToPdf(dot, baseName);
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

    // Create dead state for empty set (φ) - must be after start state
    set<int> emptySet;
    subsetMap[emptySet] = nextId++;
    int deadStateId = subsetMap[emptySet];
    dfa.stateToSubset[deadStateId] = emptySet;
    // Add self-loops for dead state
    for (char sym : dfa.alphabet) {
        dfa.transitions[deadStateId][sym] = deadStateId;
    }

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
                // Map to dead state (φ)
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
        cout << (first ? "" : ", ") << formatStateSubset(dfa, pair.first);
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

set<int> getNfaTransition(const NFA &nfa, const set<int> &states, char sym)
{
    set<int> result;
    for (int s : states)
    {
        if (nfa.transitions.count(s) && nfa.transitions.at(s).count(sym))
        {
            for (int next : nfa.transitions.at(s).at(sym))
            {
                result.insert(next);
            }
        }
    }
    return result;
}

bool simulateNfa(const NFA &nfa, const string &input)
{
    set<int> currentStates = getEpsilonClosure(nfa, nfa.startStates);
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
        set<int> nextStates = getNfaTransition(nfa, currentStates, c);
        nextStates = getEpsilonClosure(nfa, nextStates);

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
        if (nfa.finalStates.count(s))
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

DFA inputCustomDFA()
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

        if (from >= 0 && from < dfa.numStates &&
            to >= 0 && to < dfa.numStates &&
            dfa.alphabet.count(sym))
        {
            dfa.transitions[from][sym] = to;
        }
        else
        {
            cout << "  Invalid! Try again.\n";
            i--;
        }
    }
    return dfa;
}

NFA dfaToNfa(const DFA &dfa)
{
    NFA nfa;
    nfa.numStates = dfa.numStates;
    nfa.startStates.insert(dfa.startState);
    nfa.finalStates = dfa.finalStates;
    nfa.alphabet = dfa.alphabet;

    for (int from = 0; from < dfa.numStates; from++)
    {
        if (dfa.transitions.count(from))
        {
            for (char sym : dfa.alphabet)
            {
                if (dfa.transitions.at(from).count(sym))
                {
                    int to = dfa.transitions.at(from).at(sym);
                    nfa.transitions[from][sym].insert(to);
                }
            }
        }
    }
    return nfa;
}

// ==================== Main Program ====================
int main()
{
    cout << "\n+==================================================+\n";
    cout << "|   NFA → DFA CONVERTER & SIMULATOR                  |\n";
    cout << "|   Theory of Computation - Final Project 2026       |\n";
    cout << "+====================================================+\n";

    NFA currentNFA;
    DFA currentDFA;
    int choice;

    do
    {
        cout << "\n+----------------- MAIN MENU -----------------+\n";
        cout << "| 1. NFA → DFA & Simulate                       |\n";
        cout << "| 2. DFA → NFA & Simulate                       |\n";
        cout << "| 0. Exit                                       |\n";
        cout << "+-----------------------------------------------+\n";
        cout << "Choice: ";
        if (!(cin >> choice) || choice < 0 || choice > 2)
        {
            choice = -1;
            cout << "[ERROR] Invalid Input choose from 0-2.\n";
            clearInputBuffer();
            continue;
        }

        switch (choice)
        {
        case 1:
            if (currentNFA.numStates != 0)
            {
                cout << "\n[INFO] NFA already exists.\n";
                cout << "1. Use stored NFA\n";
                cout << "2. Input new NFA\n";
                cout << "Choice: ";
                int nfaChoice;
                cin >> nfaChoice;
                if (nfaChoice == 2)
                {
                    currentNFA = inputCustomNFA();
                }
                else
                {
                    cout << "[INFO] Using stored NFA.\n";
                }
            }
            else
            {
                currentNFA = inputCustomNFA();
            }
            cout << "\n[AUTO] Converting NFA → DFA...\n";
            currentDFA = nfaToDfa(currentNFA);

            drawNFAGraphical(currentNFA);
            saveNfaToPdf(currentNFA, "NFA");
            waitForUser();

            drawDFAGraphical(currentDFA);
            saveDfaToPdf(currentDFA, "DFA");
            waitForUser();

            cout << "\n[Test Strings on DFA] Type 'exit' to stop:\n";
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

        case 2:
            if (currentDFA.numStates != 0)
            {
                cout << "\n[INFO] DFA already exists.\n";
                cout << "1. Use stored DFA\n";
                cout << "2. Input new DFA\n";
                cout << "Choice: ";
                int dfaChoice;
                cin >> dfaChoice;
                if (dfaChoice == 2)
                {
                    currentDFA = inputCustomDFA();
                }
                else
                {
                    cout << "[INFO] Using stored DFA.\n";
                }
            }
            else
            {
                currentDFA = inputCustomDFA();
            }
            cout << "\n[AUTO] Converting DFA → NFA...\n";
            currentNFA = dfaToNfa(currentDFA);

            drawNFAGraphical(currentNFA);
            saveNfaToPdf(currentNFA, "NFA_from_DFA");
            waitForUser();

            cout << "\n[Test Strings on NFA] Type 'exit' to stop:\n";
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
                    if (!currentNFA.alphabet.count(c))
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

                simulateNfa(currentNFA, input);
            }
            break;

        case 0:
            cout << "\n[INFO] Good luck with your TOC project! 🎓\n";
            break;

        default:
            cout << "[ERROR] Invalid Input choose from 0-2.\n";
            waitForUser();
        }
    } while (choice != 0);

    return 0;
}
