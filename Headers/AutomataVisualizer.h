#ifndef AUTOMATA_VISUALIZER_H
#define AUTOMATA_VISUALIZER_H

#include "NFA.h"
#include "DFA.h"
#include <string>

class AutomataVisualizer
{
public:
    static void drawNFAGraphical(const NFA &nfa);
    static void drawDFAGraphical(const DFA &dfa);
    static std::string generateNfaDot(const NFA &nfa);
    static std::string generateDfaDot(const DFA &dfa);
    static bool saveDotToPdf(const std::string &dotCode, const std::string &filename);
    static void saveNfaToPdf(const NFA &nfa, const std::string &baseName);
    static void saveDfaToPdf(const DFA &dfa, const std::string &baseName);
};

#endif
