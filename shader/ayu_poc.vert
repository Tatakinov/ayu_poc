#version 410 core
uniform mat4 mvp;
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 color;
out vec3 vertex_color;
void main() {
    vertex_color = color;
    gl_Position = mvp * position;
}
