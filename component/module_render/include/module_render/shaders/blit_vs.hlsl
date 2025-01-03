#include "binding_helpers.hlsli"
#include "types/blit_cb.h"

DECLARE_PUSH_CONSTANTS(blit_constants_t, g_Blit, 0, 0);

void main(
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