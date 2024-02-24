#version 410 core

uniform mat4 projection_matrix;

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uv;

out vec4 fragment_color;
out vec2 fragment_uv;

void main()
{
    fragment_color = color;
    fragment_uv = uv;

    gl_Position = projection_matrix * vec4(position.xy, 0, 1);
}