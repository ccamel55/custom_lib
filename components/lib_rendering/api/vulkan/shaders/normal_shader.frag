#version 450

layout(binding = 1) uniform sampler2D texture_sampler;

layout(location = 0) in vec4 fragment_color;
layout(location = 1) in vec2 fragment_uv;

layout(location = 0) out vec4 out_color;

void main() {
    const vec4 sampled_texture = texture(texture_sampler, fragment_uv);
    out_color = sampled_texture * fragment_color;
}