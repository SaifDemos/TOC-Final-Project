#include <iostream>
#include "Headers/NFA.h"
#include "Headers/DFA.h"
#include "Headers/AutomataConverter.h"
#include "Headers/AutomataSimulator.h"
#include "Headers/AutomataVisualizer.h"
#include "Headers/UserInputHandler.h"
#include "Headers/Utils.h"

using namespace std;

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
                    currentNFA = UserInputHandler::inputCustomNFA();
                else
                    cout << "[INFO] Using stored NFA.\n";
            }
            else
                currentNFA = UserInputHandler::inputCustomNFA();

            cout << "\n[AUTO] Converting NFA → DFA...\n";
            currentDFA = AutomataConverter::nfaToDfa(currentNFA);

            AutomataVisualizer::drawNFAGraphical(currentNFA);
            AutomataVisualizer::saveNfaToPdf(currentNFA, "NFA");
            waitForUser();

            AutomataVisualizer::drawDFAGraphical(currentDFA);
            AutomataVisualizer::saveDfaToPdf(currentDFA, "DFA");
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

                AutomataSimulator::simulateDfa(currentDFA, input);
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
                    currentDFA = UserInputHandler::inputCustomDFA();
                else
                    cout << "[INFO] Using stored DFA.\n";
            }
            else
                currentDFA = UserInputHandler::inputCustomDFA();

            cout << "\n[AUTO] Converting DFA → NFA...\n";
            currentNFA = AutomataConverter::dfaToNfa(currentDFA);

            AutomataVisualizer::drawNFAGraphical(currentNFA);
            AutomataVisualizer::saveNfaToPdf(currentNFA, "NFA_from_DFA");
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

                AutomataSimulator::simulateNfa(currentNFA, input);
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
