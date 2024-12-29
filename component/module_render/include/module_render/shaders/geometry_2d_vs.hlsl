#pragma pack_matrix(row_major)

cbuffer CB : register(b0) {
    float4x4 mvp_matrix;
    float4x4 model_matrix;
    float4x4 view_matrix;
    float4x4 projection_matrix;
};

void main(
    float3 i_pos : POSITION,
    float2 i_uv : UV,
    float4 i_color : COLOR,
    out float4 o_pos : SV_Position,
    out float2 o_uv : UV,
    out float4 o_color : COLOR
) {
    // Note: since we use row major, we need to do: proj * view * model
    o_pos 	= mul(float4(i_pos, 1.0), mvp_matrix);
    o_uv 	= i_uv;
    o_color = i_color;
}