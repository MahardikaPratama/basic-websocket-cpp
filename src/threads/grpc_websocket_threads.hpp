#pragma once
#include <string>

void InsertEmailThreadSafe(const std::string& email);
bool DeleteEmailThreadSafe(const std::string& email);
