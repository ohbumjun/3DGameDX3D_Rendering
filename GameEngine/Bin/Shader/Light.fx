
#include "ShaderInfo.fx"

struct PS_OUTPUT_LIGHTACC
{
    float4 Dif : SV_TARGET;
    float4 Spc : SV_TARGET1;
    float4 Emv : SV_TARGET2;
};

struct VS_OUTPUT_LIGHTACC
{
    float4 Pos : SV_POSITION;
    float4 ProjPos : POSITION;
};

Texture2DMS<float4> g_GBufferDiffuseTex : register(t14);
Texture2DMS<float4> g_GBuffer1Tex : register(t15);
Texture2DMS<float4> g_GBuffer2Tex : register(t16);
Texture2DMS<float4> g_GBuffer3Tex : register(t17);

Texture2DMS<float4> g_LightDifTex : register(t18);
Texture2DMS<float4> g_LightSpcTex : register(t19);
Texture2DMS<float4> g_LightEmvTex : register(t20);

Texture2DMS<float4> g_LightBlendTex : register(t21);
Texture2DMS<float4> g_ShadowMapTex : register(t22);

cbuffer ShadowCBuffer : register(b10)
{
    matrix g_matShadowVP;
    matrix g_matShadowInvVP;
    float  g_ShadowBias;
    float2 g_ShadowResolution;
    float  g_ShadowEmpty;
};



LightResult ComputeLight(float3 Pos, float3 Normal, float4 MaterialColor)
{
    LightResult result = (LightResult) 0;
	
    float3 LightDir = (float3) 0.f;
    float Attn = 1.f;
	
    if (g_LightType == LightTypeDir)
    {
        LightDir = -g_LightDir;
        LightDir = normalize(LightDir);
    }
	
	
    if (g_LightType == LightTypePoint)
    {
        LightDir = g_LightPos - Pos;
        LightDir = normalize(LightDir);
		
        float Dist = distance(g_LightPos, Pos);
		
        if (Dist > g_LightDistance)
            Attn = 0.f;
        else
            Attn = 1.f / (g_LightAtt1 + g_LightAtt2 * Dist + g_LightAtt3 * (Dist * Dist));
    }
	
	
    if (g_LightType == LightTypeSpot)
    {
    }
	
    float3 ViewNormal = Normal;
	
	// 내적값이 음수가 나오면 0이 반환되고 양수가 나오면 해당 값이 그대로 반환된다.
    float Intensity = max(0.f, dot(ViewNormal, LightDir));
    
    float3 MtrlDif = ConvertColor(MaterialColor.r).rgb;
    float3 MtrlAmb = ConvertColor(MaterialColor.g).rgb;
    float3 MtrlSpc = ConvertColor(MaterialColor.b).rgb;
    float3 MtrlEmv = ConvertColor(MaterialColor.a).rgb;
	
    result.Dif = g_LightColor.xyz * MtrlDif * Intensity * Attn;
    result.Amb = g_LightColor.xyz * 0.2f * MtrlAmb * Attn;
	
    float3 View = -Pos;
    View = normalize(View);
	
	// 퐁 쉐이딩
    float3 Reflect = 2.f * ViewNormal * dot(ViewNormal, LightDir) - LightDir;
    Reflect = normalize(Reflect);
    float SpcIntensity = max(0.f, dot(View, Reflect));
	
	// 블린-퐁 쉐이딩
    //float3 Reflect = normalize(View + LightDir);
    //float SpcIntensity = max(0.f, dot(ViewNormal, Reflect));
	
    result.Spc = g_LightColor.xyz * MtrlSpc *
		pow(SpcIntensity, g_MtrlSpecularColor.w) * Attn;
	
    result.Emv = MtrlEmv;
	
    return result;
}

VS_OUTPUT_LIGHTACC LightAccVS(uint VertexID : SV_VertexID)
{
    VS_OUTPUT_LIGHTACC output = (VS_OUTPUT_LIGHTACC) 0;
    
    output.ProjPos = float4(g_NullPos[VertexID], 0.f, 1.f);
    output.Pos = output.ProjPos;
    
    return output;
}

PS_OUTPUT_LIGHTACC LightAccPS(VS_OUTPUT_LIGHTACC input)
{
    PS_OUTPUT_LIGHTACC output = (PS_OUTPUT_LIGHTACC) 0;
    
    float2 UV;
    UV.x = input.ProjPos.x / input.ProjPos.w * 0.5f + 0.5f;
    UV.y = input.ProjPos.y / input.ProjPos.w * -0.5f + 0.5f;
    
    int2 TargetPos = (int2) 0;
	
    TargetPos.x = (int) (UV.x * g_Resolution.x);
    TargetPos.y = (int) (UV.y * g_Resolution.y);
	
    float4 GBufferDiffuseColor = g_GBufferDiffuseTex.Load(TargetPos, 0);
    
    if (GBufferDiffuseColor.a == 0.f)
        clip(-1);
    
    float4 GBuffer1Color = g_GBuffer1Tex.Load(TargetPos, 0);
    float4 GBuffer2Color = g_GBuffer2Tex.Load(TargetPos, 0);
    float4 GBuffer3Color = g_GBuffer3Tex.Load(TargetPos, 0);
    
    // uv를 이용해서 위치를 구해준다.
    // 먼저 투영 공간의 위치를 구해준다.
    float4 ProjPos;
    ProjPos.x = UV.x * 2.f - 1.f;
    ProjPos.y = UV.y * -2.f + 1.f;
    ProjPos.z = GBuffer2Color.r;
    ProjPos.w = 1.f;
    
    ProjPos *= GBuffer2Color.g;
    
    // 뷰공간의 위치를 구한다.
    float3 ViewPos = mul(ProjPos, g_matInvProj).xyz;
    
    float3 ViewNormal = GBuffer1Color.xyz;
    
    LightResult result = ComputeLight(ViewPos, ViewNormal, GBuffer3Color);
    
    output.Dif.rgb = result.Dif + result.Amb;
    output.Spc.rgb = result.Spc;
    output.Emv.rgb = result.Emv;
    
    output.Dif.a = 1.f;
    output.Spc.a = 1.f;
    output.Emv.a = 1.f;
    
    
    return output;
}

PSOutput_Single LightBlendPS(VS_OUTPUT_LIGHTACC input)
{
    PSOutput_Single output = (PSOutput_Single) 0;
    
    float2 UV;
    UV.x = input.ProjPos.x / input.ProjPos.w * 0.5f + 0.5f;
    UV.y = input.ProjPos.y / input.ProjPos.w * -0.5f + 0.5f;
    
    int2 TargetPos = (int2) 0;
	
    TargetPos.x = (int) (UV.x * g_Resolution.x);
    TargetPos.y = (int) (UV.y * g_Resolution.y);
    
    float4 GBufferDiffuseColor = g_GBufferDiffuseTex.Load(TargetPos, 0);
    
    if (GBufferDiffuseColor.a == 0.f)
        clip(-1);
	
    float4 LightDiffuseColor = g_LightDifTex.Load(TargetPos, 0);    
    float4 LightSpecularColor = g_LightSpcTex.Load(TargetPos, 0);
    float4 LightEmissiveColor = g_LightEmvTex.Load(TargetPos, 0);
    
    //if (LightDiffuseColor.a == 0.f)
    //    clip(-1);
    
    float4 GBuffer2Color = g_GBuffer2Tex.Load(TargetPos, 0);
    
    float4 ProjPos;
    ProjPos.x = input.ProjPos.x;
    ProjPos.y = input.ProjPos.y;
    ProjPos.z = GBuffer2Color.r;
    ProjPos.w = 1.f;
    
    ProjPos *= GBuffer2Color.g;
    
    // 월드공간으로 변환한다.
    float3 WorldPos = mul(ProjPos, g_matInvVP).xyz;
    
    // 조명공간으로 변환한다.
    float4 ShadowPos = mul(float4(WorldPos, 1.f), g_matShadowVP);
    
    float2 ShadowUV;
    ShadowUV.x = ShadowPos.x / ShadowPos.w * 0.5f + 0.5f;
    ShadowUV.y = ShadowPos.y / ShadowPos.w * -0.5f + 0.5f;
    
    int2 ShadowTargetPos = (int2) 0;
	
    ShadowTargetPos.x = (int) (ShadowUV.x * g_ShadowResolution.x);
    ShadowTargetPos.y = (int) (ShadowUV.y * g_ShadowResolution.y);
    
    float4 ShadowMap = g_ShadowMapTex.Load(ShadowTargetPos, 0);
    
    if (ShadowMap.a > 0.f)
    {
        if(ShadowPos.z - g_ShadowBias > ShadowMap.r * ShadowPos.w)
        {
            LightDiffuseColor.rgb *= 0.2f;
            LightSpecularColor.rgb *= 0.f;
        }
    }
    
    
    output.Color.rgb = GBufferDiffuseColor.rgb * LightDiffuseColor.rgb +
        LightSpecularColor.rgb + LightEmissiveColor.rgb;
    output.Color.a = GBufferDiffuseColor.a;

    
    return output;

}


PSOutput_Single LightBlendRenderPS(VS_OUTPUT_LIGHTACC input)
{
    PSOutput_Single output = (PSOutput_Single) 0;
    
    float2 UV;
    UV.x = input.ProjPos.x / input.ProjPos.w * 0.5f + 0.5f;
    UV.y = input.ProjPos.y / input.ProjPos.w * -0.5f + 0.5f;
    
    int2 TargetPos = (int2) 0;
	
    TargetPos.x = (int) (UV.x * g_Resolution.x);
    TargetPos.y = (int) (UV.y * g_Resolution.y);
	
    float4 LightBlendColor = g_LightBlendTex.Load(TargetPos, 0);
    
    if (LightBlendColor.a == 0.f)
        clip(-1);
    
    output.Color = LightBlendColor;
    
    return output;
}