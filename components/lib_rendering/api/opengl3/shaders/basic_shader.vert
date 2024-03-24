#version 410 core

layout(std140) uniform uniform_buffer_object_t
{
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 model_matrix;
} ubo;

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uv;

out vec4 fragment_color;
out vec2 fragment_uv;

void main()
{
    fragment_color = color;
    fragment_uv = uv;

    gl_Position = ubo.projection_matrix * ubo.view_matrix * ubo.model_matrix * vec4(position, 1.0);
}