#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <csignal>
#include <ctime>
#include <cstring>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cerrno>
#include <dirent.h>

class Tintin_reporter {
private:
    std::ofstream log_file;
public:
    Tintin_reporter(const std::string &path) {
        log_file.open(path.c_str(), std::ios::app);
        if (!log_file.is_open()) {
            throw std::runtime_error("Could not open log file");
        }
    }

    ~Tintin_reporter() {
        log_file.close();
    }

    void log(const std::string &message, const std::string &severity) {
        time_t now = time(0);
        struct tm tm;
        char timestamp[80];
        localtime_r(&now, &tm);
        strftime(timestamp, 80, "[%d/%m/%Y - %H:%M:%S]", &tm);
        log_file << timestamp << " [ " << severity << " ] " << message << std::endl;
    }
};

volatile sig_atomic_t shutdown_flag = 0;

void signal_handler(int sig) {
    shutdown_flag = 1;
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    if (setsid() < 0) exit(EXIT_FAILURE);
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main() {
    if (getuid() != 0) {
        std::cerr << "Must be run as root" << std::endl;
        return 1;
    }

    int lock_fd = open("/var/lock/matt_daemon.lock", O_CREAT | O_RDWR, 0644);
    if (lock_fd < 0 || flock(lock_fd, LOCK_EX | LOCK_NB) < 0) {
        std::cerr << "Error: Daemon already running or lock failed" << std::endl;
        return 1;
    }

    struct stat st;
    if (stat("/var/log/matt_daemon", &st) != 0 && mkdir("/var/log/matt_daemon", 0755) != 0) {
        std::cerr << "Error creating log directory" << std::endl;
        close(lock_fd);
        return 1;
    }

    daemonize();

    Tintin_reporter *reporter;
    try {
        reporter = new Tintin_reporter("/var/log/matt_daemon/matt_daemon.log");
    } catch (const std::exception &e) {
        close(lock_fd);
        return 1;
    }

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        reporter->log("Socket creation failed", "ERROR");
        delete reporter;
        close(lock_fd);
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in address = {AF_INET, htons(4242), INADDR_ANY};
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        reporter->log("Bind failed", "ERROR");
        close(server_fd);
        delete reporter;
        close(lock_fd);
        return 1;
    }

    listen(server_fd, 3);
    reporter->log("Daemon started and listening on port 4242", "INFO");

    std::vector<int> clients;
    fd_set read_fds;

    while (!shutdown_flag) {
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        int max_fd = server_fd;

        for (int client : clients) {
            FD_SET(client, &read_fds);
            if (client > max_fd) max_fd = client;
        }

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0 && errno != EINTR) {
            reporter->log("Select error", "ERROR");
            break;
        }

        if (FD_ISSET(server_fd, &read_fds) && clients.size() < 3) {
            int client = accept(server_fd, NULL, NULL);
            if (client >= 0) {
                clients.push_back(client);
                reporter->log("New client connected", "INFO");
            }
        }

        for (size_t i = 0; i < clients.size();) {
            int client = clients[i];
            if (FD_ISSET(client, &read_fds)) {
                char buffer[1024];
                ssize_t len = read(client, buffer, sizeof(buffer)-1);
                if (len > 0) {
                    buffer[len] = '\0';
                    std::string msg(buffer);
                    if (msg.find("quit") != std::string::npos) {
                        shutdown_flag = 1;
                        break;
                    }
                    reporter->log("Message: " + msg, "INFO");
                } else {
                    close(client);
                    clients.erase(clients.begin() + i);
                    reporter->log("Client disconnected", "INFO");
                    continue;
                }
            }
            i++;
        }
    }

    reporter->log("Daemon shutting down", "INFO");
    close(server_fd);
    for (int client : clients) close(client);
    delete reporter;
    close(lock_fd);
    remove("/var/lock/matt_daemon.lock");
    return 0;
}