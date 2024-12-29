Texture2D t_texture : register(t0);
SamplerState s_sampler : register(s0);

void main(
    in float4 pos : SV_Position,
    in float2 uv : UV,
    out float4 o_rgba : SV_Target
) {
    o_rgba = t_texture.Sample(s_sampler, uv);
}