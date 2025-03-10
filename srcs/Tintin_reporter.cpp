#include "../includes/Tintin_reporter.hpp"

Tintin_reporter::Tintin_reporter(const std::string& log_path) {
    _log_file.open(log_path.c_str(), std::ios::app);
    if (!_log_file.is_open()) {
        throw std::runtime_error("Failed to open log file: " + log_path);
    }
}

Tintin_reporter::~Tintin_reporter() {
    if (_log_file.is_open()) {
        _log_file.close();
    }
}

void Tintin_reporter::log(const std::string& message, const std::string& severity) {
    time_t now = time(0);
    struct tm tm;
    char timestamp[80];
    localtime_r(&now, &tm);
    strftime(timestamp, sizeof(timestamp), "[%d/%m/%Y-%H:%M:%S]", &tm);
    _log_file << timestamp << " [ " << severity << " ] — " << message << std::endl;
}