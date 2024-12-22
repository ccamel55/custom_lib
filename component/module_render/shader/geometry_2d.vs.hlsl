#pragma pack_matrix(row_major)

cbuffer CB : register(b0) {
    float4x4 transform;
};

void main(
    float3 i_pos : POSITION,
    float2 i_uv : UV,
    float4 i_color : COLOR,
    out float4 o_pos : SV_Position,
    out float2 o_uv : UV,
    out float4 o_color : COLOR
) {
    o_pos = mul(float4(i_pos, 1), transform);
    o_uv = i_uv;
    o_color = i_color;
}