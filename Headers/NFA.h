#ifndef NFA_H
#define NFA_H

#include <set>
#include <map>
#include <queue>
#include <string>

class NFA
{
public:
    int numStates = 0;
    std::set<int> startStates;
    std::set<int> finalStates;
    std::set<char> alphabet;
    std::map<int, std::map<char, std::set<int>>> transitions;
    std::map<int, std::set<int>> epsilonTransitions;

    std::set<int> getEpsilonClosure(const std::set<int> &states) const;
    std::set<int> getTransition(const std::set<int> &states, char sym) const;
    bool isFinal(int state) const;
};

#endif
