#include "ShaderInfo.fx"

// Shader Manager
cbuffer LightFowardCBuffer : register(b8)
{
	int     g_ForwardLightCount;
	float3 g_LightFowardEmpty;
};

struct LightInfo
{
	float4 LightColor;
	int LightType;
	float3 LightPos;
	float3 LightDir;
	float LightDistance;
	float LightAngleIn;
	float LightAngleOut;
	float LightAtt1;
	float LightAtt2;
	float LightAtt3;
	float3 LightEmpty;
};

// LightManager -> CreateLightListStructuredBuffer
StructuredBuffer<LightInfo> g_LightInfoArray : register(t13);

LightResult ComputeLightFromStructuredBuffer(LightInfo Info, float3 Pos, float3 Normal, float2 UV)
{
	LightResult result = (LightResult)0;

	float3 LightDir = (float3)0.f;
	float Attn = 1.f;

	if (Info.LightType == LightTypeDir)
	{
		// LightDir = -Info.LightDir;
		LightDir = -Info.LightDir;
		LightDir = normalize(LightDir);
	}

	else if (Info.LightType == LightTypePoint)
	{
		// 현재 위치 --> Light 방향 
		LightDir = Info.LightPos - Pos;
		LightDir = normalize(LightDir);

		float Dist = distance(Info.LightPos, Pos);

		if (Dist > Info.LightDistance)
			Attn = 0.f;
		else
			Attn = 1.f / (Info.LightAtt1 + Info.LightAtt2 * Dist + Info.LightAtt3 * (Dist * Dist));
	}

	// 뷰공간 상에 정의된 Normal
	float3 ViewNormal = Normal;

	// 해당 위치 --> 눈 방향
	float3 View = -Pos;
	View = normalize(View);

	// 난반사광 강도 
	float DiffuseIntensity = max(0.f, dot(ViewNormal, LightDir));

	// 퐁 쉐이딩
	float3 Reflect = 2.f * ViewNormal * dot(ViewNormal, LightDir) - LightDir;
	Reflect = normalize(Reflect);

	float SpcIntensity = max(0.f, dot(View, Reflect));

	// float4 BaseTextureColor = g_BaseTexture.Sample(g_BaseSmp, UV.xy);
	// BaseTextureColor.rgb = BaseTextureColor.rgb * g_MtrlBaseColor.rgb;
	// float3 MtrlDif = BaseTextureColor.rgb * g_MtrlBaseColor.rgb;

	float3 MtrlDif = g_MtrlBaseColor.rgb;
	float3 MtrlAmb = g_MtrlAmbientColor.rgb;

	float3 MtrlSpc = g_MtrlSpecularColor.rgb;

	if (g_MtrlSpecularTex)
	{
		MtrlSpc = g_SpecularTexture.Sample(g_BaseSmp, UV).rgb;
	}

	float3 MtrlEmv = g_MtrlEmissiveColor.rgb;

	if (g_MtrlEmissiveTex)
	{
		MtrlEmv = g_EmissiveTexture.Sample(g_BaseSmp, UV).rgb;
	}

	// 감마 공간 -> Linear 공간
	// MtrlDif = pow(MtrlDif, 2.2f);
	
	result.Dif = Info.LightColor.xyz * MtrlDif * DiffuseIntensity * Attn;
	// result.Amb = Info.LightColor.xyz * g_GLightAmbIntensity * MtrlAmb * Attn;

	result.Amb = Info.LightColor.xyz * 1.f * MtrlAmb * Attn;

	// Linear Space -> Gamma Space
	// result.Dif = pow(result.Dif, 1 / 2.2f);

	// g_MtrlSpecularColor.w : 표면 까끌까끌한 정도
	result.Spc = Info.LightColor.xyz * MtrlSpc * pow(SpcIntensity, g_MtrlSpecularColor.w) * Attn;

	result.Emv = MtrlEmv;

	return result;
}