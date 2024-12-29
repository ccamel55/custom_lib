Texture2D t_texture : register(t0);
SamplerState s_sampler : register(s0);

void main(
    in float4 i_pos : SV_Position,
    in float2 i_uv : UV,
    in float4 i_color : COLOR,
    out float4 o_color : SV_Target0
) {
    o_color = t_texture.Sample(s_sampler, i_uv) * i_color;
}