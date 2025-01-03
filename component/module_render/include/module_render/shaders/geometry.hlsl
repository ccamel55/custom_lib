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
    o_color = t_texture.Sample(s_sampler, i_uv) * i_color;
}