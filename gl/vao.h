#ifndef VAO_H_
#define VAO_H_

#include <GL/gl.h>
#include <memory>

class VAO {
    private:
        int n_;
        std::unique_ptr<GLuint[]> vao;
    public:
        VAO() : n_(1) {
            vao = std::make_unique<GLuint[]>(n_);
            glGenVertexArrays(n_, vao.get());
        }
        ~VAO() {
            glDeleteVertexArrays(n_, vao.get());
        }
        void bind() const {
            glBindVertexArray(vao[0]);
        }
        void attribute(int index, GLint size) {
            glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(index);
        }
};

#endif // VAO_H_
