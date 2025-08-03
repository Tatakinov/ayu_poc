#ifndef SHAPE_H_
#define SHAPE_H_

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "program.h"
#include "object.h"

class Shape {
    private:
        std::shared_ptr<Object> obj_;
        int size_;
        Program program_;
        glm::mat4 projection_;
        glm::mat4 view_;
        glm::mat4 model_;
        glm::mat4 mvp_;
        GLuint id_;
    protected:
        GLsizei vertex_count_;
    public:
        Shape(const std::string path, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &colors, const std::vector<GLuint> indices) : size_(vertices.size()) {
            program_.load(path + "shader/ayu_poc.vert", path + "shader/ayu_poc.frag");
            obj_ = std::make_shared<Object>(3, vertices, colors, indices);
            projection_ = glm::perspective(glm::radians(45.0), 1.0, 0.1, 100.0);
            view_ = glm::lookAt(
                    glm::vec3(0, 0, 5),
                    glm::vec3(0, 0, 0),
                    glm::vec3(0, 1, 0)
                    );
            id_ = glGetUniformLocation(program_, "mvp");
            model_ = glm::mat4(1.0f);
        }

        void draw() {
            program_.use();
            obj_->bind();
            execute();
        }

        void execute() {
            model_ = glm::rotate(model_, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            mvp_ = projection_ * view_ * model_;
            glUniformMatrix4fv(id_, 1, GL_FALSE, glm::value_ptr(mvp_));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        ~Shape() {}
};

#endif // SHAPE_H_
