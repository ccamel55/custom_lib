#if TEXTURE_ARRAY
Texture2DArray t_texture : register(t0);
#else
Texture2D t_texture : register(t0);
#endif
SamplerState s_sampler : register(s0);

void main(
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