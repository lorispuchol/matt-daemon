    #include "../includes/Tintin_reporter.hpp"

    Tintin_reporter::Tintin_reporter(const std::string & log_path) {
        this->_log_file.open(log_path.c_str(), std::ios::app);
        if (!this->_log_file.is_open()) {
            throw std::runtime_error("Failed to open log file: " + log_path);
        }
    }

    Tintin_reporter::Tintin_reporter(const Tintin_reporter & src) {
        *this = src;
    }

    Tintin_reporter &Tintin_reporter::operator=(const Tintin_reporter & src) {
        (void)src;
        return *this;
    }

    Tintin_reporter &Tintin_reporter::operator<<(const std::string & message) {
        this->log(message, "INFO");
        return *this;
    }

    Tintin_reporter::~Tintin_reporter() {
        if (this->_log_file.is_open()) {
            this->_log_file.close();
        }
    }

    void Tintin_reporter::log(const std::string & message, const std::string & severity) {
        time_t now = time(0);
        struct tm tm;
        char timestamp[80];
        localtime_r(&now, &tm);
        strftime(timestamp, sizeof(timestamp), "[%d/%m/%Y-%H:%M:%S]", &tm);
        this->_log_file << timestamp << " [ " << severity << " ] — " << message << std::endl;
        _log_file.flush();
    }