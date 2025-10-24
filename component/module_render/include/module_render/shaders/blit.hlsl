#include "binding_helpers.hlsli"
#include "types/blit_cb.h"

DECLARE_PUSH_CONSTANTS(blit_constants_t, g_Blit, 0, 0);

void main_vs(
    in uint iVertex : SV_VertexID,
    out float4 o_posClip : SV_Position,
    out float2 o_uv : UV
) {
    uint u = iVertex & 1;
    uint v = (iVertex >> 1) & 1;

    float2 src_uv = float2(u, v) * g_Blit.s_size + g_Blit.s_origin;
    float2 dst_uv = float2(u, v) * g_Blit.d_size + g_Blit.d_origin;

    o_posClip = float4(dst_uv.x * 2 - 1, 1 - dst_uv.y * 2, 0, 1);
    o_uv = src_uv;
}

#if TEXTURE_ARRAY
Texture2DArray t_texture : register(t0);
#else
Texture2D t_texture : register(t0);
#endif
SamplerState s_sampler : register(s0);

void main_ps(
    in float4 pos : SV_Position,
    in float2 uv : UV,
    out float4 o_rgba : SV_Target
) {
    #if TEXTURE_ARRAY
    o_rgba = t_texture.Sample(s_sampler, float3(uv, 0));
    #else
    o_rgba = t_texture.Sample(s_sampler, uv);
    #endif
}