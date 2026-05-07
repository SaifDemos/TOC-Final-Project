#ifndef AUTOMATA_CONVERTER_H
#define AUTOMATA_CONVERTER_H

#include "NFA.h"
#include "DFA.h"

class AutomataConverter
{
public:
    static DFA nfaToDfa(const NFA &nfa);
    static NFA dfaToNfa(const DFA &dfa);
};

#endif
