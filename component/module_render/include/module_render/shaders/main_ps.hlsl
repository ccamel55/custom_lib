void main(
    in float4 i_pos : SV_Position,
    in float3 i_color : COLOR,
    out float4 o_color : SV_Target0
) {
    o_color = float4(i_color, 1);
}