#include "gl/window.h"

#include "gl/ayu_.h"

void Window::resize(int width, int height) {
    int w, h;
    glfwGetFramebufferSize(window_, &w, &h);
    glViewport(0, 0, w, h);
    std::unique_lock<std::mutex> lock(mutex_);
    size_[0] = static_cast<GLfloat>(width);
    size_[1] = static_cast<GLfloat>(height);
}

void Window::focus(int focused) {
    focused_ = (focused == GLFW_TRUE);
}

void Window::position(int x, int y) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        position_ = {static_cast<double>(x), static_cast<double>(y)};
    }
    resetBalloonPosition();
}

void Window::mouseButton(int button, int action, int mods) {
    mouse_state_[button].press = (action == GLFW_PRESS);
    if (button == GLFW_MOUSE_BUTTON_LEFT && !mouse_state_[button].press) {
        drag_.reset();
    }
    if (!mouse_state_[button].press && !mouse_state_[button].drag) {
        int x = position_.x + cursor_position_.x;
        int y = position_.y + cursor_position_.y;
        int b = 0;
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            b = 0;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            b = 1;
        }
        else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
            b = 2;
        }
        std::vector<std::string> args;
        args = {to_s(x), to_s(y), to_s(0), to_s(side_), "", to_s(b)};

        auto now = std::chrono::system_clock::now();
        auto prev = mouse_state_[button].prev;
        mouse_state_[button].prev = now;

        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - prev).count() < 300) {
#if 1
            Request req = {"NOTIFY", "OnMouseDoubleClick", args};
            parent_->enqueueDirectSSTP({req});
#endif
        }
        else if (button != GLFW_MOUSE_BUTTON_RIGHT) {
            Request up = {"NOTIFY", "OnMouseUp", args};
            Request click = {"NOTIFY", "OnMouseClick", args};
            parent_->enqueueDirectSSTP({up, click});
        }
        else {
            Request up = {"NOTIFY", "OnMouseUp", args};
            Request click = {"NOTIFY", "OnMouseClick", args};
#if 0
            // 右クリックメニューを呼び出す
            args = {to_s(side_), to_s(x), to_s(y)};
            Request menu = {"EXECUTE", "OpenMenu", args};
            parent_->enqueueDirectSSTP({up, click, menu});
#else
            parent_->enqueueDirectSSTP({up, click});
#endif
        }
    }
    for (auto &[k, v] : mouse_state_) {
        if (!v.press) {
            v.drag = false;
        }
    }
}

void Window::cursorPosition(double x, double y) {
    if (!drag_.has_value() && mouse_state_[GLFW_MOUSE_BUTTON_LEFT].press) {
        drag_ = std::make_optional<Position>(cursor_position_);
    }
    cursor_position_ = {x, y};
    if (drag_.has_value()) {
        auto [dx, dy] = drag_.value();
        glfwSetWindowPos(window_, position_.x + x - dx, position_.y + y - dy);
    }
    for (auto &[k, v] : mouse_state_) {
        if (v.press) {
            v.drag = true;
        }
    }
}

void Window::resetBalloonPosition() {
    auto p = getPosition();
    //auto offset = getBalloonOffset
    int x, y;
    // 右
    if (direction_) {
        Position size = getSize();
        // x = p.x + size.x - offset.x
        x = p.x + size.x;
    }
    // 左
    else {
        // x = p.x - w + offset.x
        auto res = parent_->sendDirectSSTP("EXECUTE", "GetBalloonSize", {to_s(side_)});
        std::string content = res.getContent();
        int w = 0, h = 0;
        char delim;
        if (!content.empty()) {
            std::istringstream iss(content);
            iss >> w;
            iss >> delim;
            iss >> h;
        }
        x = p.x - w;
    }
    // y = p.y + offset.y
    y = p.y;
    parent_->sendDirectSSTP("EXECUTE", "SetBalloonPosition", {to_s(side_), to_s(x), to_s(y)});
}
