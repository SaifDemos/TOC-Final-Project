#include "../Headers/Utils.h"
#include <iostream>
#include <limits>
#include <iomanip>

using namespace std;

void clearInputBuffer()
{
    // Clear failbit but NOT eofbit - let EOF persist
    cin.clear(cin.rdstate() & ~ios::failbit);
    // Only ignore if not EOF
    if (!cin.eof())
    {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void waitForUser()
{
    cout << "\n[Press Enter to continue...]";
    cin.get();
}

string trim(const string &s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

bool readInt(int &out, int minVal, int maxVal, const string &errorMsg)
{
    int temp;
    // Read input into temp to avoid modifying out on failure (C++11+ sets out=0 on failed extract)
    if (!(cin >> temp))
    {
        // Failed to read - check if EOF
        if (cin.eof())
        {
            cout << "[ERROR] Unexpected end of input.\n";
        }
        else
        {
            cout << errorMsg;
        }
        clearInputBuffer();
        return false;
    }
    // Validate range
    if (temp < minVal || temp > maxVal)
    {
        cout << errorMsg;
        clearInputBuffer();
        return false;
    }
    out = temp;
    return true;
}

bool readPositiveInt(int &out, const string &errorMsg)
{
    int temp;
    // Read into temp to avoid modifying out on failure
    if (!(cin >> temp))
    {
        // Failed to read - check if EOF
        if (cin.eof())
        {
            cout << "[ERROR] Unexpected end of input.\n";
        }
        else
        {
            cout << errorMsg;
        }
        clearInputBuffer();
        return false;
    }
    // Validate positive
    if (temp <= 0)
    {
        cout << errorMsg;
        clearInputBuffer();
        return false;
    }
    out = temp;
    return true;
}

bool readChar(char &out, const string &errorMsg)
{
    char temp;
    if (!(cin >> temp))
    {
        if (cin.eof())
        {
            cout << "[ERROR] Unexpected end of input.\n";
        }
        else
        {
            cout << errorMsg;
        }
        clearInputBuffer();
        return false;
    }
    out = temp;
    return true;
}
