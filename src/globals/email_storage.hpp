#pragma once
#include <set>
#include <string>
#include <mutex>

extern std::set<std::string> email_storage;
extern std::mutex storage_mutex;
