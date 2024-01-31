#version 450

layout(location = 0) in vec4 fragment_color;
layout(location = 1) in vec2 fragment_tex_pos;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = fragment_color;
}