#include "ShaderInfo.fx"

struct VS_TONEMAP_OUTPUT
{
    float4 Pos  : SV_Position; // vertex pos
    float2 UV   : TEXCOORD0;
};

// 중간 회색과 흰색 값은, 장면의 모습에 따라, 날씨에 따라 , 낮과 밤에 따라 다르게 설정해야 한다.
cbuffer FinalPassConstants : register(s7)
{
    float g_MiddleGrey;
    float g_LumWhiteSqr;
    float2 g_FinalPassEmpty;
}

// 휘도 계산을 위한 상수 
static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);

// Texture2D<float4> HDRTexture : register(t51);
Texture2DMS<float4> HDRTex : register(t21);
StructuredBuffer<float> AvgLum	: register(t15); // 읽기 전용

float3 ToneMapping(float3 vHDRColor)
{
    // 현재 픽셀에 대한 휘도 스케일 계산
    float fLScale = dot(vHDRColor, LUM_FACTOR);
    fLScale *= g_MiddleGrey / AvgLum[0];
    fLScale = (fLScale + fLScale * fLScale / g_LumWhiteSqr) / (1.f + fLScale);

    // 휘도 스케일을 픽셀 색상에 적용
    return vHDRColor * fLScale;
}


VS_TONEMAP_OUTPUT ToneMappingVS(uint VertexID : SV_VertexID)
{
    VS_TONEMAP_OUTPUT output = (VS_TONEMAP_OUTPUT)0;

    output.Pos = float4(g_NullPos[VertexID], 0.f, 1.f);
    output.UV = g_NullUV[VertexID].xy;

    return output;
}

// Tone Mapping 공식은 MiddleGrey와 LunWhiteSqr을 통해 밝기를 조절할 수 있다.
PSOutput_Single ToneMappingPS(VS_TONEMAP_OUTPUT Input)
{
    PSOutput_Single output = (PSOutput_Single)0;

    // 색상 샘플링 (HDR 색)
    // float3 vColor = HDRTex.Sample(g_BaseSmp, Input.UV.xy).xyz;
    // float3 vColor = HDRTex.Load(g_BaseSmp, Input.UV.xy).xyz;
    float3 vColor = HDRTex.Load(Input.UV.xy, 0).xyz;

    // 톤 매핑(HDR 색을 LDR색으로 변환)
    vColor = ToneMapping(vColor);

    output.Color = float4(vColor, 1.f);

    // LDR 값 출력
    return output;
}
