#ifndef MISC_H_
#define MISC_H_

#include <vector>

struct Position {
    double x, y;
};

struct Request {
    std::string method;
    std::string command;
    std::vector<std::string> args;
};

#endif // MISC_H_
