#pragma once

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

#define PORT 4242
#define LOCK_FILE "/var/lock/matt_daemon.lock"
#define MAX_CLIENTS 3
#define LOG_DIR "/var/log/matt_daemon/"
#define LOG_FILE "matt_daemon.log"