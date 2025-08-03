#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <GL/gl.h>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

class Program {
    private:
        GLuint id_;

        std::string loadFile(std::string path) {
            std::ifstream ifs(path);
            std::string buffer;
            // HACK 0x00はテキストファイルには現れないのでそれを利用して
            // 全読み込みを行う
            std::getline(ifs, buffer, '\0');
            return buffer;
        }

        bool createProgram(std::string vertex, std::string fragment) {
            int success;
            char log[1024];
            id_ = glCreateProgram();
            const GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
            const char *vstr = vertex.c_str();
            glShaderSource(vshader, 1, &vstr, NULL);
            glCompileShader(vshader);
            glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(vshader, 1024, NULL, log);
                //std::cout << "Error.Shader: " << log << std::endl;
                return false;
            }

            glAttachShader(id_, vshader);
            glDeleteShader(vshader);

            const GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
            const char *fstr = fragment.c_str();
            glShaderSource(fshader, 1, &fstr, NULL);
            glCompileShader(fshader);
            glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(fshader, 1024, NULL, log);
                //std::cout << "Error.Shader: " << log << std::endl;
                return false;
            }
            glAttachShader(id_, fshader);
            glDeleteShader(fshader);

            glBindAttribLocation(id_, 0, "position");
            glBindAttribLocation(id_, 1, "color");
            glBindFragDataLocation(id_, 0, "fragment");
            glLinkProgram(id_);
            glGetProgramiv(id_, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(id_, 1024, NULL, log);
                //std::cout << "Error.Program: " << log << std::endl;
                return false;
            }
            return true;
        }
    public:
        Program() {}
        ~Program() {
            glDeleteProgram(id_);
        }
        bool load(std::string vertex, std::string fragment) {
            auto v = loadFile(vertex);
            auto f = loadFile(fragment);
            return createProgram(v, f);
        }
        void use() const {
            glUseProgram(id_);
        }
        operator GLuint() {
            return id_;
        }
};
#endif // PROGRAM_H_
