#pragma once

#include <fstream>
#include <string>
#include <stdexcept>
#include <ctime>

class Tintin_reporter {
public:

    Tintin_reporter() = delete;
    explicit Tintin_reporter(const std::string & log_path);

    Tintin_reporter(const Tintin_reporter & src);
    Tintin_reporter& operator=(const Tintin_reporter & src);
    Tintin_reporter& operator<<(const std::string & message);

    ~Tintin_reporter();
    
    // Maybe remove severity ?
    void log(const std::string & message, const std::string & severity);

private:
    std::ofstream _log_file;
};