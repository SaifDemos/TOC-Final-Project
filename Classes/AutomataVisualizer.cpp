#include "../Headers/AutomataVisualizer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>

using namespace std;

void AutomataVisualizer::drawNFAGraphical(const NFA &nfa)
{
    if (nfa.numStates <= 0)
    {
        cout << "\n[WARN] No NFA to display (numStates <= 0)\n";
        return;
    }

    cout << "\n+==================================================+\n";
    cout << "|                 NFA GRAPHICAL VIEW                 |\n";
    cout << "+====================================================+\n";

    cout << "    ";
    for (int i = 0; i < nfa.numStates; i++)
    {
        if (nfa.startStates.count(i))
            cout << "->";
        cout << (nfa.isFinal(i) ? "[*]" : "[ ]") << "q" << i;
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

    cout << "\nTRANSITIONS:\n" << string(50, '-') << "\n";
    for (int i = 0; i < nfa.numStates; i++)
    {
        for (char sym : nfa.alphabet)
        {
            auto it = nfa.transitions.find(i);
            if (it != nfa.transitions.end())
            {
                auto sit = it->second.find(sym);
                if (sit != it->second.end())
                {
                    for (int target : sit->second)
                        cout << "  q" << i << " --" << sym << "--> q" << target << "\n";
                }
            }
        }
        auto eit = nfa.epsilonTransitions.find(i);
        if (eit != nfa.epsilonTransitions.end())
        {
            for (int target : eit->second)
                cout << "  q" << i << " --ε--> q" << target << "\n";
        }
    }

    cout << "\n" << string(60, '=') << "\n";
    cout << "TRANSITION TABLE:\n" << string(60, '=') << "\n";

    int numStates = min(nfa.numStates, 20); // Cap for display
    vector<vector<string>> matrix(numStates, vector<string>(numStates, "-"));
    for (int i = 0; i < numStates; i++)
    {
        for (char sym : nfa.alphabet)
        {
            auto it = nfa.transitions.find(i);
            if (it != nfa.transitions.end())
            {
                auto sit = it->second.find(sym);
                if (sit != it->second.end())
                {
                    for (int target : sit->second)
                    {
                        if (target >= 0 && target < numStates)
                        {
                            if (matrix[i][target] == "-")
                                matrix[i][target] = string(1, sym);
                            else
                                matrix[i][target] += "," + string(1, sym);
                        }
                    }
                }
            }
        }
    }

    cout << "      ";
    for (int j = 0; j < numStates; j++)
        cout << " q" << j << "  ";
    cout << "\n" << string(5 + numStates * 5, '-') << "\n";

    for (int i = 0; i < numStates; i++)
    {
        cout << " q" << i << " |";
        for (int j = 0; j < numStates; j++)
            cout << " " << (matrix[i][j] == "-" ? "-" : matrix[i][j]) << "  ";
        cout << "\n";
    }
}

void AutomataVisualizer::drawDFAGraphical(const DFA &dfa)
{
    if (dfa.numStates <= 0)
    {
        cout << "\n[WARN] No DFA to display (numStates <= 0)\n";
        return;
    }

    cout << "\n+==================================================+\n";
    cout << "|                 DFA GRAPHICAL VIEW                |\n";
    cout << "+==================================================+\n";

    cout << "    ";
    for (int i = 0; i < dfa.numStates; i++)
    {
        if (i == dfa.startState)
            cout << "->";
        cout << (dfa.isFinal(i) ? "[*]" : "[ ]") << dfa.formatStateSubset(i);
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

    cout << "\n" << string(60, '=') << "\n";
    cout << "TRANSITION TABLE:\n" << string(60, '=') << "\n";

    int maxWidth = 3;
    int numStates = min(dfa.numStates, 20); // Cap for display
    for (int i = 0; i < numStates; i++)
    {
        string rep = dfa.formatStateSubset(i);
        maxWidth = max(maxWidth, (int)rep.length());
        auto it = dfa.transitions.find(i);
        if (it != dfa.transitions.end())
        {
            for (char sym : dfa.alphabet)
            {
                auto sit = it->second.find(sym);
                if (sit != it->second.end())
                {
                    string target = dfa.formatStateSubset(sit->second);
                    maxWidth = max(maxWidth, (int)target.length());
                }
            }
        }
    }
    int colWidth = max(maxWidth + 4, 7);
    colWidth = min(colWidth, 30); // Cap width

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

    for (int i = 0; i < numStates; i++)
    {
        cout << "| " << left << setw(colWidth - 2) << dfa.formatStateSubset(i);
        for (char sym : dfa.alphabet)
        {
            int next = dfa.getTransition(i, sym);
            if (next != -1)
                cout << "| " << setw(colWidth - 2) << dfa.formatStateSubset(next);
            else
                cout << "| " << setw(colWidth - 2) << "-";
        }
        cout << "|\n";
    }
    cout << "+" << string(colWidth, '-');
    for (size_t i = 0; i < dfa.alphabet.size(); i++)
        cout << "+" << string(colWidth, '-');
    cout << "+\n";

    cout << "\nVISUAL FLOW:\n" << string(50, '-') << "\n";
    for (int i = 0; i < numStates; i++)
    {
        for (char sym : dfa.alphabet)
        {
            int t = dfa.getTransition(i, sym);
            if (t != -1)
            {
                cout << "  " << dfa.formatStateSubset(i) << " --" << sym << "--> " << dfa.formatStateSubset(t);
                if (t == i)
                    cout << " (self-loop)";
                if (dfa.isFinal(t))
                    cout << " [FINAL]";
                cout << "\n";
            }
        }
    }
}

string AutomataVisualizer::generateNfaDot(const NFA &nfa)
{
    stringstream dot;
    dot << "digraph NFA {\n";
    dot << "  rankdir=LR;\n";
    dot << "  node [shape=circle, style=filled, fillcolor=\"#E8F4F8\"];\n";
    dot << "  edge [fontsize=10];\n\n";
    dot << "  start [shape=plaintext, label=\"\"];\n\n";

    for (int i = 0; i < nfa.numStates; i++)
    {
        dot << "  q" << i;
        if (nfa.isFinal(i))
            dot << " [shape=doublecircle]";
        dot << ";\n";
    }
    dot << "\n";

    for (int s : nfa.startStates)
        dot << "  start -> q" << s << ";\n";
    dot << "\n";

    for (int i = 0; i < nfa.numStates; i++)
    {
        for (char sym : nfa.alphabet)
        {
            auto it = nfa.transitions.find(i);
            if (it != nfa.transitions.end())
            {
                auto sit = it->second.find(sym);
                if (sit != it->second.end())
                {
                    for (int target : sit->second)
                        dot << "  q" << i << " -> q" << target << " [label=\"" << sym << "\"];\n";
                }
            }
        }
        auto eit = nfa.epsilonTransitions.find(i);
        if (eit != nfa.epsilonTransitions.end())
        {
            for (int target : eit->second)
                dot << "  q" << i << " -> q" << target << " [label=\"ε\", style=dashed];\n";
        }
    }

    dot << "}\n";
    return dot.str();
}

string AutomataVisualizer::generateDfaDot(const DFA &dfa)
{
    stringstream dot;
    dot << "digraph DFA {\n";
    dot << "  rankdir=LR;\n";
    dot << "  node [shape=circle, style=filled, fillcolor=\"#E8F4F8\"];\n";
    dot << "  edge [fontsize=10];\n\n";
    dot << "  start [shape=plaintext, label=\"\"];\n\n";

    for (int i = 0; i < dfa.numStates; i++)
    {
        dot << "  state" << i;
        if (dfa.isFinal(i))
            dot << " [shape=doublecircle]";
        dot << " [label=\"" << dfa.formatStateSubset(i) << "\"]";
        dot << ";\n";
    }
    dot << "\n";

    dot << "  start -> state" << dfa.startState << ";\n\n";

    for (int i = 0; i < dfa.numStates; i++)
    {
        auto it = dfa.transitions.find(i);
        if (it != dfa.transitions.end())
        {
            for (char sym : dfa.alphabet)
            {
                auto sit = it->second.find(sym);
                if (sit != it->second.end())
                    dot << "  state" << i << " -> state" << sit->second
                        << " [label=\"" << sym << "\"];\n";
            }
        }
    }

    dot << "}\n";
    return dot.str();
}

bool AutomataVisualizer::saveDotToPdf(const string &dotCode, const string &filename)
{
    string dotFile = filename + ".dot";
    string pdfFile = filename + ".pdf";

    ofstream out(dotFile);
    if (!out)
    {
        cout << "[ERROR] Cannot create " << dotFile << "\n";
        return false;
    }
    out << dotCode;
    out.close();

    string cmd = "dot -Tpdf " + dotFile + " -o " + pdfFile;
    int result = system(cmd.c_str());

    remove(dotFile.c_str());

    if (result != 0)
    {
        cout << "[ERROR] dot command failed. Is Graphviz installed?\n";
        return false;
    }

    cout << "[INFO] Saved to " << pdfFile << "\n";
    return true;
}

void AutomataVisualizer::saveNfaToPdf(const NFA &nfa, const string &baseName)
{
    string dot = generateNfaDot(nfa);
    saveDotToPdf(dot, baseName);
}

void AutomataVisualizer::saveDfaToPdf(const DFA &dfa, const string &baseName)
{
    string dot = generateDfaDot(dfa);
    saveDotToPdf(dot, baseName);
}
