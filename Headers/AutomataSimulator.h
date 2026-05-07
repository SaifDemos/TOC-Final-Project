#ifndef AUTOMATA_SIMULATOR_H
#define AUTOMATA_SIMULATOR_H

#include "NFA.h"
#include "DFA.h"

class AutomataSimulator
{
public:
    static bool simulateDfa(const DFA &dfa, const std::string &input);
    static bool simulateNfa(const NFA &nfa, const std::string &input);
};

#endif
