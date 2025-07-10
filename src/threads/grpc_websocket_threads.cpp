#include "threads/grpc_websocket_threads.hpp"
#include <set>
#include <mutex>

static std::set<std::string> g_email_storage;
static std::mutex g_storage_mutex;

void InsertEmailThreadSafe(const std::string& email) {
    std::lock_guard<std::mutex> lock(g_storage_mutex);
    g_email_storage.insert(email);
}

bool DeleteEmailThreadSafe(const std::string& email) {
    std::lock_guard<std::mutex> lock(g_storage_mutex);
    return g_email_storage.erase(email) > 0;
}
