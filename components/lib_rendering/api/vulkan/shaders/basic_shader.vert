#version 450

layout(binding = 0) uniform uniform_buffer_object_t
{
    mat4 projection_matrix;
    mat4 view_matrix;
} ubo;

layout(push_constant) uniform push_constants_t
{
    mat4 model_matrix;
} pc;

layout(location = 0) in vec3 in_pos;
layout(location = 1) in uint in_color;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec4 fragment_color;
layout(location = 1) out vec2 fragment_uv;

void main() {
    fragment_color = unpackUnorm4x8(in_color);
    fragment_uv = in_uv;

    gl_Position = ubo.projection_matrix * ubo.view_matrix * pc.model_matrix * vec4(in_pos, 1.0);
}