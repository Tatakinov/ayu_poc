#ifndef OBJECT_H_
#define OBJECT_H_

#include <GL/gl.h>

#include "gl/ebo.h"
#include "gl/vao.h"
#include "gl/vbo.h"

class Object {
    private:
        VBO vbo_[2];
        EBO ebo_;
        VAO vao_;
    public:
        Object(GLint size, const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &colors, const std::vector<GLuint> &indices) : vbo_(), vao_() {
            vao_.bind();
            vbo_[0].bind(vertices);
            vao_.attribute(0, size);
            vbo_[1].bind(colors);
            vao_.attribute(1, size);
            ebo_.bind(indices);
        }
        void bind() const {
            vao_.bind();
        }
        ~Object() {}
};

#endif // OBJECT_H_
