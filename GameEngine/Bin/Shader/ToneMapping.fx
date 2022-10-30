#include "PostProcessingInfo.fx"

struct VS_TONEMAP_OUTPUT
{
    float4 Pos         : SV_POSITION; // vertex pos
    float4 ProjPos   : POSITION;
};

// 중간 회색과 흰색 값은, 장면의 모습에 따라, 날씨에 따라 , 낮과 밤에 따라 다르게 설정해야 한다.
cbuffer FinalPassConstants : register(b13)
{
    uint g_MiddleGrey;
    uint g_LumWhiteSqr;
    float2 g_FinalPassEmpty;
};

// Texture2D<float4> HDRTexture : register(t51);
StructuredBuffer<float> AvgLum	: register(t35); // 읽기 전용

float4 ToneMapping(float3 vHDRColor)
{
    // 현재 픽셀에 대한 휘도 스케일 계산
    // 중간 회색값
    float LScale = dot(vHDRColor, LUM_FACTOR.xyz); // 이거는 문제 X

    // fLScale *= g_MiddleGrey / AvgLum[0];
    LScale *= g_MiddleGrey / AvgLum[0];

    float FinalScale = (float)0.f;

    // 분자 
    float Numerator = LScale;
    Numerator += (LScale * LScale) / (g_LumWhiteSqr * g_LumWhiteSqr);
    // Numerator += (LScale * LScale) / (g_LumWhiteSqr * g_LumWhiteSqr);
    
    // 분모
    float Denominator = 1.f + LScale;
    FinalScale = Numerator / Denominator;

    // 문제 1 : 상수 버퍼 값을 제대로 읽어오지 못한다. (해결 => 상수버퍼 형식을 uint 로 했어야 했는데 float 으로 해버렸었다)
    // 문제 2 : AvgLum[0] 가 0 인 것 같다. 
    // -> 2_1) 계산 셰이더에서 잘못 계산해주거나  
    // - DownScaleBuffer 를 한번만 Update 해줘서 그런건가 ?
    // => 이게 유력하다. 현재 거의 0이 나온다. (계산 셰이더 참고) 
    // 2_2. 셰이더 리소스 뷰로 읽어오지 못하거나

    // 휘도 스케일을 픽셀 색상에 적용
    return float4(vHDRColor * FinalScale, 1.f);
    // return float4(vHDRColor * FinalScale, 1.f);
    // return vHDRColor * 1;
}


VS_TONEMAP_OUTPUT ToneMappingVS(uint VertexID : SV_VertexID)
{
    VS_TONEMAP_OUTPUT output = (VS_TONEMAP_OUTPUT)0;

    output.ProjPos = float4(g_NullPos[VertexID], 0.f, 1.f);
    output.Pos = output.ProjPos;

    return output;
}

// Tone Mapping 공식은 MiddleGrey와 LunWhiteSqr을 통해 밝기를 조절할 수 있다.
PSOutput_Single ToneMappingPS(VS_TONEMAP_OUTPUT Input)
{
    PSOutput_Single output = (PSOutput_Single)0;

    float2 UV = (float2) 0;
    UV.x = Input.ProjPos.x / Input.ProjPos.w * 0.5f + 0.5f;
    UV.y = Input.ProjPos.y / Input.ProjPos.w * -0.5f + 0.5f;

    int2 TargetPos = (int2) 0;
    TargetPos.x = (int)(UV.x * g_Resolution.x);
    TargetPos.y = (int)(UV.y * g_Resolution.y);

    // 색상 샘플링 (HDR 색)
    float4 vColor = HDRTex.Load(TargetPos, 0);

    // 톤 매핑(HDR 색을 LDR색으로 변환)
    vColor = ToneMapping(vColor.xyz);

    output.Color = vColor;

    // LDR 값 출력
    return output;
}
