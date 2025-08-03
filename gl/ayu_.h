#ifndef GL_AYU_H_
#define GL_AYU_H_

#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "gl/misc.h"
#include "gl/util.h"
#include "gl/window.h"

class Window;

class Ayu {
    private:
        std::mutex mutex_;
        std::condition_variable cond_;
        std::unordered_map<int, std::shared_ptr<Window>> windows_;
        std::queue<std::vector<std::string>> queue_;
        std::queue<std::vector<Request>> event_queue_;
        std::unique_ptr<std::thread> th_recv_;
        std::unique_ptr<std::thread> th_send_;
        std::string ayu_dir_;
#if !defined(_WIN32) && !defined(WIN32)
        std::string path_;
        std::string uuid_;
#endif // WIN32
        bool alive_;

    public:
        Ayu() : alive_(true) {}
        ~Ayu();

        bool init();

        bool create(int side);

        Position getPosition(int side);

        Position getSize(int side);

        void resetBalloonPosition();

        void setBalloonOffset(int side, int x, int y);
        Position getBalloonOffset(int side);

        void show(int side);

        void hide(int side);

        operator bool() {
            std::unordered_set<int> set;
            for (auto &[k, v] : windows_) {
                if (!v) {
                    set.emplace(k);
                }
            }
            for (auto e : set) {
                windows_.erase(e);
            }
            return alive_;
        }

        void draw();

        sstp::Response sendDirectSSTP(std::string method, std::string command, std::vector<std::string> args) {
            sstp::Request req {method};
            sstp::Response res {500, "Internal Server Error"};
            req["Charset"] = "UTF-8";
            req["Ayu"] = uuid_;
            if (path_.empty()) {
                return res;
            }
            req["Sender"] = "AYU_PoC";
            req["Option"] = "nodescript";
            if (req.getCommand() == "EXECUTE") {
                req["Command"] = command;
            }
            else if (req.getCommand() == "NOTIFY") {
                req["Event"] = command;
            }
            for (int i = 0; i < args.size(); i++) {
                req(i) = args[i];
            }
            sockaddr_un addr;
            if (path_.length() >= sizeof(addr.sun_path)) {
                return res;
            }
            int soc = socket(AF_UNIX, SOCK_STREAM, 0);
            if (soc == -1) {
                return res;
            }
            memset(&addr, 0, sizeof(sockaddr_un));
            addr.sun_family = AF_UNIX;
            // null-terminatedも書き込ませる
            strncpy(addr.sun_path, path_.c_str(), path_.length() + 1);
            if (connect(soc, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)) == -1) {
                return res;
            }
            std::string request = req;
            if (write(soc, request.c_str(), request.size()) != request.size()) {
                close(soc);
                return res;
            }
            char buffer[BUFFER_SIZE] = {};
            std::string data;
            while (true) {
                int ret = read(soc, buffer, BUFFER_SIZE);
                if (ret == -1) {
                    close(soc);
                    return res;
                }
                if (ret == 0) {
                    close(soc);
                    break;
                }
                data.append(buffer, ret);
            }
            return sstp::Response::parse(data);
        }

        void enqueueDirectSSTP(std::vector<Request> list);
};

#endif // GL_AYU_H_
