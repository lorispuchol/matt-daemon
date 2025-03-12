#pragma once

#include <fstream>
#include <string>
#include <stdexcept>
#include <ctime>
#include <sys/stat.h>

class Tintin_reporter {
public:

    Tintin_reporter() = delete;
    explicit Tintin_reporter(const std::string & path, const std::string & name);

    Tintin_reporter(const Tintin_reporter & src);
    Tintin_reporter& operator=(const Tintin_reporter & src);
    Tintin_reporter& operator<<(const std::string & message);

    ~Tintin_reporter();
    
    // Maybe remove severity ?
    void log(const std::string & message, const std::string & severity);

private:

    void _ensure_log_available(void);

    std::ofstream       _log_file;
    const std::string   _log_dir;
    const std::string   _log_name;
};