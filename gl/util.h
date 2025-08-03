#ifndef UTIL_H_
#define UTIL_H_

#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "sstp.h"

constexpr int BUFFER_SIZE = 1024;

template<typename T>
void to_x(std::string s, T &value) {
    std::istringstream iss(s);
    iss >> value;
}

template<typename T>
std::string to_s(T value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

#endif // UTIL_H_
