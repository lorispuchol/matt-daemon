    #include "../includes/Tintin_reporter.hpp"

    Tintin_reporter::Tintin_reporter(const std::string & path, const std::string & name): _log_dir(path), _log_name(name) {
        this->_log_file.open((this->_log_dir + this->_log_name).c_str(), std::ios::app);
        if (!this->_log_file.is_open()) {
            throw std::runtime_error("Failed to open log file: " + path + name);
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

    void Tintin_reporter::_ensure_log_available() {
        struct stat buffer;

        if (stat(this->_log_dir.c_str(), &buffer) != 0) {
            mkdir(this->_log_dir.c_str(), 0755);
        }

        if (stat((this->_log_dir + this->_log_name).c_str(), &buffer) != 0) {
            this->_log_file.close();  // Close the old stream (if open)
            this->_log_file.open((this->_log_dir + this->_log_name).c_str(), std::ios::app);  // Recreate the file
            if (!this->_log_file.is_open()) {
                return ;
            }
        }
    }

    void Tintin_reporter::log(const std::string & message, const std::string & severity) {

        this->_ensure_log_available();

        time_t now = time(0);
        struct tm tm;
        char timestamp[80];
        localtime_r(&now, &tm);
        strftime(timestamp, sizeof(timestamp), "[%d/%m/%Y-%H:%M:%S]", &tm);
        if (this->_log_file.is_open()) {
            this->_log_file << timestamp << " [ " << severity << " ] — " << message << std::endl;
            _log_file.flush();
        }
    }