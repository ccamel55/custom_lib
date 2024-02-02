#version 450

layout(binding = 0) uniform uniform_buffer_object_t
{
    vec2 scale;
    vec2 translate;
} ubo;

layout(location = 0) in vec2 in_pos;
layout(location = 1) in uint in_color;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec4 fragment_color;
layout(location = 1) out vec2 fragment_uv;

const vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

const vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

//void main() {
//    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
//    fragment_color = colors[gl_VertexIndex];
//}

void main() {
    fragment_color = unpackUnorm4x8(in_color);
    fragment_uv = in_uv;

    gl_Position = vec4((in_pos * ubo.scale) + ubo.translate, 0.0, 1.0);
}