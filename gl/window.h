#ifndef WINDOW_H_
#define WINDOW_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>

#include "gl/misc.h"
#include "gl/shape.h"
#include "gl/ayu_.h"

class Ayu;

namespace {
    const std::vector<glm::vec3> vertices = {
        glm::vec3( 1.0,  1.0,  1.0),
        glm::vec3(-1.0,  1.0,  1.0),
        glm::vec3(-1.0,  1.0, -1.0),
        glm::vec3( 1.0,  1.0, -1.0),
        glm::vec3( 1.0, -1.0,  1.0),
        glm::vec3(-1.0, -1.0,  1.0),
        glm::vec3(-1.0, -1.0, -1.0),
        glm::vec3( 1.0, -1.0, -1.0),
    };

    const std::vector<glm::vec3> colors = {
        glm::vec3( 0.0,  0.0,  0.0),
        glm::vec3( 1.0,  0.0,  0.0),
        glm::vec3( 0.0,  1.0,  0.0),
        glm::vec3( 0.0,  0.0,  1.0),
        glm::vec3( 1.0,  1.0,  0.0),
        glm::vec3( 0.0,  1.0,  1.0),
        glm::vec3( 1.0,  0.0,  1.0),
        glm::vec3( 1.0,  1.0,  1.0),
    };

    const std::vector<GLuint> indices = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4,

        0, 1, 5,
        5, 4, 0,

        1, 2, 6,
        6, 5, 1,

        2, 3, 7,
        7, 6, 2,

        3, 0, 4,
        4, 7, 3,
    };
}

class Window {
    struct State {
        bool press;
        bool drag;
        std::chrono::system_clock::time_point prev;
    };
    private:
        std::mutex mutex_;
        GLFWwindow *window_;
        GLfloat size_[2];
        GLfloat scale_;
        Position position_;
        bool focused_;
        bool dragged_;
        std::unordered_map<int, State> mouse_state_;
        std::optional<Position> drag_;
        Position cursor_position_;
        std::unique_ptr<Shape> shape_;
        bool direction_;
        Position balloon_offset_;
        Ayu *parent_;
        int side_;

        static void resizeCallback(GLFWwindow *window, int width, int height) {
            auto instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
            if (instance == NULL) {
                return;
            }
            instance->resize(width, height);
        }

        void resize(int width, int height);

        static void focusCallback(GLFWwindow *window, int focused) {
            auto instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
            if (instance == NULL) {
                return;
            }
            instance->focus(focused);
        }

        void focus(int focused);

        static void positionCallback(GLFWwindow *window, int x, int y) {
            auto instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
            if (instance == NULL) {
                return;
            }
            instance->position(x, y);
        }

        void position(int x, int y);

        static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
            auto instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
            if (instance == NULL) {
                return;
            }
            instance->mouseButton(button, action, mods);
        }

        void mouseButton(int button, int action, int mods);

        static void cursorPositionCallback(GLFWwindow *window, double x, double y) {
            auto instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
            if (instance == NULL) {
                return;
            }
            instance->cursorPosition(x, y);
        }

        void cursorPosition(double x, double y);

        static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
            auto instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
            if (instance == NULL) {
                return;
            }
            instance->key(key, scancode, action, mods);
        }

        void key(int key, int scancode, int action, int mods);

    public:
        Window(Ayu *parent, int side) : window_(nullptr), scale_(100), dragged_(false), direction_(false), balloon_offset_({0, 0}), parent_(parent), side_(side) {}
        virtual ~Window() {
            if (window_ != nullptr) {
                glfwDestroyWindow(window_);
            }
        }
        bool create(const std::string ayu_dir, int width, int height, const char *title) {
            if (window_ != nullptr) {
                return true;
            }
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
            //Debian bookwormじゃまだ対応してない
            //glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);

            window_ = glfwCreateWindow(width, height, title, NULL, NULL);
            if (window_ == nullptr) {
                return false;
            }

            glfwMakeContextCurrent(window_);

            if (glewInit() != GLEW_OK) {
                return false;
            }

            glfwSwapInterval(1);

            glfwSetWindowUserPointer(window_, this);

            glfwSetWindowSizeCallback(window_, resizeCallback);
            glfwSetWindowPosCallback(window_, positionCallback);
            glfwSetWindowFocusCallback(window_, focusCallback);
            glfwSetMouseButtonCallback(window_, mouseButtonCallback);
            glfwSetCursorPosCallback(window_, cursorPositionCallback);
            glfwSetKeyCallback(window_, keyCallback);

            resizeCallback(window_, width, height);

            glClearDepth(1.0);
            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);

            shape_ = std::make_unique<Shape>(ayu_dir, vertices, colors, indices);

            return true;
        }

        operator bool() {
            return !glfwWindowShouldClose(window_);
        }

        void draw() {
            glfwMakeContextCurrent(window_);
            glClearColor(0.0, 0.0, 0.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shape_->draw();
            glfwMakeContextCurrent(nullptr);
        }

        void swapBuffers() const {
            glfwSwapBuffers(window_);
        }

        const GLfloat *getSize() const {
            return size_;
        }

        GLfloat getScale() const {
            return scale_;
        }

        void show() {
            glfwShowWindow(window_);
        }

        void hide() {
            glfwHideWindow(window_);
        }

        Position getPosition() {
            std::unique_lock<std::mutex> lock(mutex_);
            Position p = {position_.x, position_.y};
            return p;
        }

        Position getSize() {
            std::unique_lock<std::mutex> lock(mutex_);
            Position p = {size_[0], size_[1]};
            return p;
        }

        void setBalloonOffset(int x, int y) {
            balloon_offset_ = {x, y};
        }

        Position getBalloonOffset() const {
            return balloon_offset_;
        }

        void resetBalloonPosition();
};

#endif // WINDOW_H_
