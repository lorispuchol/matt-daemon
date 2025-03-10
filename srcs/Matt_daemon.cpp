#include "../includes/Matt_daemon.hpp"
#include "../includes/Tintin_reporter.hpp"

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

    int lock_fd = open(LOCK_FILE,  O_CREAT | O_RDWR, 0644);
    if (lock_fd < 0 || flock(lock_fd, LOCK_EX | LOCK_NB) < 0) {
        std::cerr << "Can't open :/var/lock/matt_daemon.lock" << std::endl;
        return 1;
    }

    struct stat st;
    if (stat(LOG_DIR, &st) != 0 && mkdir(LOG_DIR, 0755) != 0) {
        std::cerr << "Error creating log directory" << std::endl;
        close(lock_fd);
        return 1;
    }

    daemonize();

    Tintin_reporter *reporter = nullptr;
    try {
        reporter = new Tintin_reporter(LOG_DIR LOG_FILE);
    } catch (const std::exception & e) {
        std::cerr << "Error: " << e.what() << std::endl;
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
    struct sockaddr_in address = {AF_INET, htons(PORT), INADDR_ANY};
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        reporter->log("Bind failed", "ERROR");
        close(server_fd);
        delete reporter;
        close(lock_fd);
        return 1;
    }

    listen(server_fd, 3);
    reporter->log("Daemon started and listening on port " + std::to_string(PORT), "INFO");

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

        int select_result = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (select_result < 0) {
            if (errno == EINTR) {
                // Signal interrupted select: check shutdown_flag immediately
                if (shutdown_flag) {
                    break; // Exit loop if flag is set
                } else {
                    continue; // Re-check loop condition
                }
            } else {
                reporter->log("Select error", "ERROR");
                break;
            }
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
    remove(LOCK_FILE);
    return 0;
}