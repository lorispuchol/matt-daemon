#pragma once

#include <fstream>
#include <string>
#include <stdexcept>
#include <ctime>

class Tintin_reporter {
public:
    // Constructor with log file path
    explicit Tintin_reporter(const std::string& log_path);

    // Coplien Form: Delete copy constructor and assignment operator
    Tintin_reporter(const Tintin_reporter&) = delete;
    Tintin_reporter& operator=(const Tintin_reporter&) = delete;

    // Destructor
    ~Tintin_reporter();
    
    // Maybe remove severity
    void log(const std::string& message, const std::string& severity);

private:
    std::ofstream _log_file;
};