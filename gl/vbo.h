#ifndef VBO_H_
#define VBO_H_

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>

class VBO {
    private:
        int n_;
        std::unique_ptr<GLuint[]> vbo;
    public:
        VBO() : n_(1) {
            vbo = std::make_unique<GLuint[]>(n_);
            glGenBuffers(n_, vbo.get());
        }
        ~VBO() {
            glDeleteBuffers(n_, vbo.get());
        }
        void bind(const std::vector<glm::vec3> &vertices) {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
        }
};

#endif // VBO_H_
