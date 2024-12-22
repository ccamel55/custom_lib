void main(
    in float4 i_pos : SV_Position,
    in float2 i_uv : UV,
    in float4 i_color : COLOR,
    out float4 o_color : SV_Target0
) {
    // o_color = t_Texture.Sample(s_Sampler, i_uv);
    o_color = i_color;
}