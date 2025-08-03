#include "gl/ayu_.h"

#include <cstdint>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ayu.h"
#include "gl/util.h"
#include "gl/window.h"

Ayu::~Ayu() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        alive_ = false;
    }
    th_send_->join();
    th_recv_->join();
    glfwTerminate();
}

bool Ayu::init() {
    if (glfwInit() == GLFW_FALSE) {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    th_recv_ = std::make_unique<std::thread>([&]() {
        uint32_t len;
        while (true) {
            std::cin.read(reinterpret_cast<char *>(&len), sizeof(uint32_t));
            if (std::cin.eof() || len == 0) {
                break;
            }
            char *buffer = new char[len];
            std::cin.read(buffer, len);
            std::string request(buffer, len);
            delete[] buffer;
            if (std::cin.gcount() < len) {
                break;
            }
            auto req = ayu::Request::parse(request);
            auto event = req().value();

            ayu::Response res {204, "No Content"};

            if (event == "Initialize") {
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    ayu_dir_ = req(0).value();
                }
            }
            else if (event == "Endpoint") {
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    path_ = req(0).value();
                    uuid_ = req(1).value();
                }
                cond_.notify_one();
            }
            else if (event == "Position") {
                int side;
                std::istringstream iss(req(0).value());
                iss >> side;
                res = {200, "OK"};
                Position size = getPosition(side);
                res(0) = static_cast<int>(size.x);
                res(1) = static_cast<int>(size.y);
            }
            else if (event == "Size") {
                int side;
                std::istringstream iss(req(0).value());
                iss >> side;
                res = {200, "OK"};
                Position size = getSize(side);
                res(0) = static_cast<int>(size.x);
                res(1) = static_cast<int>(size.y);
            }
            else if (event == "ResetBalloonPosition") {
                resetBalloonPosition();
            }
            else {
                std::vector<std::string> args;
                args.push_back(event);
                for (int i = 0; ; i++) {
                    if (req(i)) {
                        args.push_back(req(i).value());
                    }
                    else {
                        break;
                    }
                }
                std::unique_lock<std::mutex> lock(mutex_);
                queue_.push(args);
            }

            res["Charset"] = "UTF-8";

            std::string response = res;
            len = response.size();
            std::cout.write(reinterpret_cast<char *>(&len), sizeof(uint32_t));
            std::cout.write(response.c_str(), len);
        }
        {
            std::unique_lock<std::mutex> lock(mutex_);
            alive_ = false;
            event_queue_.push({{"", "", {}}});
        }
        cond_.notify_one();
    });

    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { return !path_.empty(); });
    }

    th_send_ = std::make_unique<std::thread>([&]() {
        while (true) {
            std::vector<Request> list;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cond_.wait(lock, [this] { return !event_queue_.empty(); });
                if (!alive_) {
                    break;
                }
                list = event_queue_.front();
                event_queue_.pop();
            }
            for (auto &request : list) {
                auto res = sendDirectSSTP(request.method, request.command, request.args);
                if (res.getStatusCode() != 204) {
                    break;
                }
            }
        }
    });

    return true;
}

bool Ayu::create(int side) {
    if (windows_.contains(side)) {
        return true;
    }
    windows_[side] = std::make_shared<Window>(this, side);
    return windows_[side]->create(ayu_dir_, 640, 480, "Ayu");
}

void Ayu::show(int side) {
    if (!windows_.contains(side)) {
        if (!create(side)) {
            return;
        }
    }
    windows_[side]->show();
}

void Ayu::hide(int side) {
    if (!windows_.contains(side)) {
        if (!create(side)) {
            return;
        }
    }
    windows_[side]->hide();
}

void Ayu::draw() {
    glfwPollEvents();
    {
        while (true) {
            std::vector<std::string> args;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if (queue_.empty()) {
                    break;
                }
                args = queue_.front();
                queue_.pop();
            }
            if (args[0] == "Create") {
                int side;
                std::istringstream iss(args[1]);
                iss >> side;
                create(side);
            }
            else if (args[0] == "Show") {
                int side;
                std::istringstream iss(args[1]);
                iss >> side;
                show(side);
                resetBalloonPosition();
            }
        }
    }
    for (auto &[_, v] : windows_) {
        v->draw();
        v->swapBuffers();
    }
}
Position Ayu::getPosition(int side) {
    Position p = {0, 0};
    if (!windows_.contains(side)) {
        if (!create(side)) {
            return p;
        }
    }
    return windows_[side]->getPosition();
}

Position Ayu::getSize(int side) {
    Position p = {0, 0};
    if (!windows_.contains(side)) {
        if (!create(side)) {
            return p;
        }
    }
    return windows_[side]->getSize();
}

void Ayu::resetBalloonPosition() {
    for (auto &[k, v] : windows_) {
        v->resetBalloonPosition();
    }
}

void Ayu::enqueueDirectSSTP(std::vector<Request> list) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        event_queue_.push(list);
    }
    cond_.notify_one();
}
