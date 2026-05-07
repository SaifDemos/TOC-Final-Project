#ifndef DFA_H
#define DFA_H

#include <set>
#include <map>
#include <string>

class DFA
{
public:
    int numStates = 0;
    int startState = -1;
    std::set<int> finalStates;
    std::set<char> alphabet;
    std::map<int, std::map<char, int>> transitions;
    std::map<int, std::set<int>> stateToSubset;

    std::string formatStateSubset(int stateId) const;
    int getTransition(int from, char sym) const;
    bool isFinal(int state) const;
};

#endif
