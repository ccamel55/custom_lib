#version 410 core

uniform sampler2D texture_sample;

in vec4 fragment_color;
in vec2 fragment_uv;

layout (location = 0) out vec4 out_color;

void main()
{
    vec4 sampled_texture = texture(texture_sample, fragment_uv.st);
    out_color = sampled_texture * fragment_color;
}