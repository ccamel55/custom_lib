#version 410 core

// best sharpness = 0.25 / (spread * scale)
// = 0.25 / (4 * 1)
const float smoothing = 1.0 / 16.0;

uniform sampler2D texture_sample;

in vec4 fragment_color;
in vec2 fragment_uv;

layout (location = 0) out vec4 out_color;

void main()
{
    vec4 sampled_texture = texture(texture_sample, fragment_uv.st);
    float outline_factor = smoothstep(0.5 - smoothing, 0.5 + smoothing, sampled_texture.a);

    vec4 sdf_texture = vec4(sampled_texture.rgb, outline_factor);
    out_color = sdf_texture * fragment_color;
}