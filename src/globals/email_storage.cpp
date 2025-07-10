#include "globals/email_storage.hpp"

std::set<std::string> email_storage;
std::mutex storage_mutex;
