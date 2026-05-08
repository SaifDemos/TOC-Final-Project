#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <limits>

void clearInputBuffer();
void waitForUser();
std::string trim(const std::string &s);

// Input validation helpers
bool readInt(int &out, int minVal = std::numeric_limits<int>::min(), int maxVal = std::numeric_limits<int>::max(), const std::string &errorMsg = "[ERROR] Invalid input\n");
bool readPositiveInt(int &out, const std::string &errorMsg = "[ERROR] Enter a positive number\n");
bool readChar(char &out, const std::string &errorMsg = "[ERROR] Invalid character\n");

#endif
