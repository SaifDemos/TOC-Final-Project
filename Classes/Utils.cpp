#include "../Headers/Utils.h"
#include <iostream>
#include <limits>
#include <iomanip>

using namespace std;

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
