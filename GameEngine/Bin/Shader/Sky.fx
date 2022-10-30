#include "ShaderInfo.fx"

struct VS_SKY_OUTPUT
{
    float4 Pos : SV_Position;
    float3 UV : TEXCOORD;
};

VS_SKY_OUTPUT SkyVS(float3 Pos : POSITION)
{
    VS_SKY_OUTPUT output = (VS_SKY_OUTPUT)0;

    output.Pos = mul(float4(Pos, 1.f), g_matWVP).xyww;
    output.UV = Pos;

    return output;
}

float4 SkyPS(VS_SKY_OUTPUT input) : SV_TARGET
{
    float4 Color = g_SkyTex.Sample(g_BaseSmp, input.UV);

    return Color;
}

/*

#include "ShaderInfo.fx"

struct VS_SKY_INPUT
{
    // 변수 뒤에 : 레지스터이름 + 번호 로 지정한다.
    // 번호를 안붙이면 0으로 지정된다.
    float3 Pos : POSITION; // Vector3타입.
    // float3 Normal : NORMAL;
    // float3 Tangent : TANGENT;
    // float3 Binormal : BINORMAL;
};

struct VS_SKY_OUTPUT
{
    float4 Pos : SV_Position;
    float3 UV : TEXCOORD;
    float4 ProjPos : POSITION;
    float3 ViewPos : POSITION1;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
};

VS_SKY_OUTPUT SkyVS(VS_SKY_INPUT Info)
// Vertex3DOutput Standard3DVS(Vertex3D input)
{
    VS_SKY_OUTPUT output = (VS_SKY_OUTPUT) 0;

    // output.Pos = mul(float4(Pos, 1.f), g_matWVP).xyww;
    // output.UV = Pos;
    float3 Pos = Info.Pos;

    output.ProjPos = mul(float4(Pos, 1.f), g_matWVP);
    output.Pos = output.ProjPos;

    // 뷰공간의 위치를 만들어준다.
    output.ViewPos = mul(float4(Pos, 1.f), g_matWV).xyz;

    // 뷰 공간의 Normal을 만들어준다.
    // output.Normal = normalize(mul(float4(Info.Normal, 0.f), g_matWV).xyz);
    output.Normal = normalize(mul(float4(0.f, 0.f, 1.f, 0.f), g_matWV).xyz);
    // 뷰 공간의 Tangent을 만들어준다.
    output.Tangent = normalize(mul(float4(1.f, 0.f, 0.f, 0.f), g_matWV).xyz);
    // 뷰 공간의 Binormal을 만들어준다.
    output.Binormal = normalize(mul(float4(0.f, 1.f, 0.f, 0.f), g_matWV).xyz);

    output.UV = output.Pos.xyz;


    return output;
}

// float4 SkyPS(VS_SKY_OUTPUT input)   : SV_TARGET
PSOutput_GBuffer SkyPS(VS_SKY_OUTPUT input)
{
    // input.Pos => NDC 로 변환되어서 들어온다.

    PSOutput_GBuffer output = (PSOutput_GBuffer)0;

    float4 BaseDiffuseColor = g_SkyTex.Sample(g_BaseSmp, input.UV);

    output.Diffuse.rgb = BaseDiffuseColor.rgb;
    output.Diffuse.a = BaseDiffuseColor.a * g_MtrlOpacity;

    // output.GBuffer1.rgb = ComputeBumpNormal(input.Normal, input.Tangent, input.Binormal, input.UV); //
    // output.GBuffer1.rgb = ComputeBumpNormal(float3(0.f, 0.f, 1.f), float3(1.0f, 0.f, 0.f), float3(0.f, 1.f, 0.f), float2(0.f, 0.f));
    output.GBuffer1.rgb = float3(0.f, 0.f, 1.f);
    output.GBuffer1.a = 1.f;

    output.GBuffer2.r = input.Pos.z / input.Pos.w;
    output.GBuffer2.g = input.Pos.w;
    output.GBuffer2.b = g_MtrlSpecularColor.w;
    output.GBuffer2.a = 1.f;

    output.GBuffer3.r = ConvertColor(g_MtrlBaseColor);
    output.GBuffer3.g = ConvertColor(g_MtrlAmbientColor);

    // output.GBuffer4.rgb = input.Tangent.xyz; //
    output.GBuffer4.rgb = float3(1.0f, 0.f, 0.f).xyz;
    output.GBuffer4.a = g_MtrlReceiveDecal;

    // output.GBuffer5.rgb = input.Binormal.xyz; //
    output.GBuffer5.rgb = float3(0.f, 1.f, 0.f).xyz;
    output.GBuffer5.a = 1.f;

    float4 SpecularColor = g_MtrlSpecularColor.xyzw;

    //if (g_MtrlSpecularTex)
    //    SpecularColor = g_SpecularTexture.Sample(g_BaseSmp, input.UV).xxxx;

    output.GBuffer3.b = ConvertColor(SpecularColor);

    float4 EmissiveColor = g_MtrlEmissiveColor.xyzw;

    // if (g_MtrlEmissiveTex)
    //     EmissiveColor = g_EmissiveTexture.Sample(g_BaseSmp, input.UV).xxxx;

    output.GBuffer3.a = ConvertColor(EmissiveColor);

    return output;

}

*/