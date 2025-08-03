#ifndef EBO_H_
#define EBO_H_

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>

class EBO {
    private:
        int n_;
        std::unique_ptr<GLuint[]> ebo;
    public:
        EBO() : n_(1) {
            ebo = std::make_unique<GLuint[]>(n_);
            glGenBuffers(n_, ebo.get());
        }
        ~EBO() {
            glDeleteBuffers(n_, ebo.get());
        }
        void bind(const std::vector<GLuint> &indices) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLfloat), &indices[0], GL_STATIC_DRAW);
        }
};

#endif // EBO_H_
