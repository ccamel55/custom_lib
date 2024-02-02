#version 450

layout(binding = 1) uniform sampler2D texture_sampler;

layout(location = 0) in vec4 fragment_color;
layout(location = 1) in vec2 fragment_uv;

layout(location = 0) out vec4 out_color;

// best sharpness = 0.25 / (spread * scale)
// = 0.25 / (4 * 1)
const float smoothing = 1.0 / 16.0;

void main() {
    const vec4 sampled_texture = texture(texture_sampler, fragment_uv);
    const float outline_factor = smoothstep(0.5 - smoothing, 0.5 + smoothing, sampled_texture.a);

    const vec4 sdf_texture = vec4(sampled_texture.rgb, outline_factor);
    out_color = sdf_texture * fragment_color;
}