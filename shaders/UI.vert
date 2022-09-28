#version 410 core

layout (location = 0) in vec2 position;

void main() {
    //Draw in front of everything
    gl_Position = vec4(position, 0, 1.0);
}