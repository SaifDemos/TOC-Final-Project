#ifndef USER_INPUT_HANDLER_H
#define USER_INPUT_HANDLER_H

#include "NFA.h"
#include "DFA.h"

class UserInputHandler
{
public:
    static void showInputGuide();
    static NFA inputCustomNFA();
    static DFA inputCustomDFA();
};

#endif
