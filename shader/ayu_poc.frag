#version 410 core
in vec3 vertex_color;
layout (location = 0) out vec4 fragment;
void main() {
    fragment = vec4(vertex_color, 1.0);
}
