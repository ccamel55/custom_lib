#pragma pack_matrix(row_major)

#include "types/geometry_cb.h"

cbuffer CB : register(b0) {
    constant_buffer_t cb;
}

void main_vs(
    float3 i_pos : POSITION,
    float2 i_uv : UV,
    float4 i_color : COLOR,
    out float4 o_pos : SV_Position,
    out float2 o_uv : UV,
    out float4 o_color : COLOR
) {
    // Note: since we use row major, we need to do: proj * view * model
    o_pos 	= mul(float4(i_pos, 1.0), cb.mvp_matrix);
    o_uv 	= i_uv;
    o_color = i_color;
}

Texture2D t_texture : register(t0);
SamplerState s_sampler : register(s0);

void main_ps(
    in float4 i_pos : SV_Position,
    in float2 i_uv : UV,
    in float4 i_color : COLOR,
    out float4 o_color : SV_Target0
) {
#if SDF_ENABLE == 0
    // Normal, no sdf rendering
    o_color = t_texture.Sample(s_sampler, i_uv) * i_color;
#else
    // best sharpness = 0.25 / (spread * scale)
    const float smoothing = 0.25f / (4.f * 1.f);

    const float4 sampled_texture = t_texture.Sample(s_sampler, i_uv);

    const float distance = sampled_texture.a;
    const float outline_factor = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);

#if SDF_ENABLE == 2
    // Between 0 and 0.5, 0 = thick outline, 0.5 = no outline
    const float outline_distance = 0.4;

    // outline will always be black for now, can change later
    const float4 outline_color = float4(0.0, 0.0, 0.0, 1.0);

    const float4 color = lerp(outline_color, sampled_texture, outline_factor);
    const float alpha = smoothstep(outline_distance - smoothing, outline_distance + smoothing, distance);

#else
    const float4 color = sampled_texture;
    const float alpha = outline_factor;
#endif

    o_color = float4(color.rgb, alpha) * i_color;
#endif
}