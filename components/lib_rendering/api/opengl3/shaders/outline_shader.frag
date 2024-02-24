#version 410 core

// best sharpness = 0.25 / (spread * scale)
// = 0.25 / (4 * 1)
const float smoothing = 1.0 / 16.0;

// Between 0 and 0.5, 0 = thick outline, 0.5 = no outline
const float outline_distance = 0.4;

// outline will always be black for now, can change later
const vec4 outline_color = vec4(0.0, 0.0, 0.0, 1.0);

uniform sampler2D texture_sample;

in vec4 fragment_color;
in vec2 fragment_uv;

layout (location = 0) out vec4 out_color;

void main()
{
    vec4 sampled_texture = texture(texture_sample, fragment_uv.st);

    float distance = sampled_texture.a;
    float outline_factor = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);

    vec4 color = mix(outline_color, sampled_texture, outline_factor);
    float alpha = smoothstep(outline_distance - smoothing, outline_distance + smoothing, distance);

    vec4 sdf_texture = vec4(color.rgb, alpha);
    out_color = sdf_texture * fragment_color;
}