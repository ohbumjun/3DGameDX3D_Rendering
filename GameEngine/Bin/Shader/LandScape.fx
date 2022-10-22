
#include "ShaderInfo.fx"

struct Vertex3D
{
	// 변수 뒤에 : 레지스터이름 + 번호 로 지정한다.
	// 번호를 안붙이면 0으로 지정된다.
    float3 Pos : POSITION; // Vector3타입.
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    float4 BlendWeight : BLENDWEIGHT;
    float4 BlendIndex : BLENDINDEX;
};

struct Vertex3DOutput
{
	// SV가 붙으면 System Value이다. 이 값은 레지스터에 저장만 하고
	// 가져다 사용하면 안된다.
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
    float4 ProjPos : POSITION;
    float3 ViewPos : POSITION1;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
};


cbuffer LandScape : register(b11)
{
    float g_LandScapeDetailLevel;
    int g_LandScapeSplatCount;
    float2 g_LandScapeEmpty;
};

Texture2DArray g_SplatDiffuse : register(t30);
Texture2DArray g_SplatNormal : register(t31);
Texture2DArray g_SplatSpecular : register(t32);
Texture2DArray g_SplatAlpha : register(t33);

float3 ComputeSplatBumpNormal(float3 Normal, float3 Tangent, 
    float3 Binormal, float2 UV, float2 OriginUV)
{
    float3 result = Normal;
	
    if (g_MtrlBumpEnable == 1)
    {
        float4 NormalColor = g_NormalTexture.Sample(g_BaseSmp, UV);
		
        for (int i = 0; i < g_LandScapeSplatCount; ++i)
        {
            float4 SplatNormal = g_SplatNormal.Sample(g_BaseSmp, float3(UV, i));
        
            float4 SplatAlpha = g_SplatAlpha.Sample(g_BaseSmp, float3(OriginUV, i));
        
            NormalColor.rgb = NormalColor.rgb * (1.f - SplatAlpha.r) + SplatNormal.rgb * SplatAlpha.r;
        }
        
		// 색상은 0 ~ 1 사이이므로 -1 ~ 1 사이의 법선벡터로 만들어준다.
        float3 ConvertNormal = NormalColor.xyz * 2.f - 1.f;
		// z는 무조건 + 방향으로 만들어준다.
        ConvertNormal.z = 1.f;
        ConvertNormal = normalize(ConvertNormal);
		
		// Tangent, Binormal, Normal을 이용해서 위에서 만들어준 법선
		// 벡터를 뷰공간으로 변환해준다. 뷰공간이 되는 이유는 Tangent,
		// Binormal, Normal 3개 모두 뷰공간으로 변환된 벡터가 들어오기
		// 때문이다.
        float3x3 mat =
        {
            Tangent,
			Binormal,
			Normal
        };

        result = normalize(mul(ConvertNormal, mat));
    }
	
    return result;
}


Vertex3DOutput LandScapeVS(Vertex3D input)
{
    Vertex3DOutput output = (Vertex3DOutput) 0;
    
    float3 Pos = input.Pos;

    output.ProjPos = mul(float4(Pos, 1.f), g_matWVP);
    output.Pos = output.ProjPos;
    
    // 뷰공간의 위치를 만들어준다.
    output.ViewPos = mul(float4(Pos, 1.f), g_matWV).xyz;
    
    // 뷰 공간의 Normal을 만들어준다.
    output.Normal = normalize(mul(float4(input.Normal, 0.f), g_matWV).xyz);
    // 뷰 공간의 Tangent을 만들어준다.
    output.Tangent = normalize(mul(float4(input.Tangent, 0.f), g_matWV).xyz);
    // 뷰 공간의 Binormal을 만들어준다.
    output.Binormal = normalize(mul(float4(input.Binormal, 0.f), g_matWV).xyz);
    output.UV = input.UV;

    return output;
}

PSOutput_GBuffer LandScapePS(Vertex3DOutput input)
{
    PSOutput_GBuffer output = (PSOutput_GBuffer) 0;
    
    float2 UV = input.UV * g_LandScapeDetailLevel;
    
    //LightResult LightInfo = ComputeLight(input.ViewPos, input.Normal,
    //    input.Tangent, input.Binormal, input.UV);

    float4 BaseTextureColor = g_BaseTexture.Sample(g_BaseSmp, UV);
    
    for (int i = 0; i < g_LandScapeSplatCount; ++i)
    {
        float4 SplatDiffuse = g_SplatDiffuse.Sample(g_BaseSmp, float3(UV, i));
        
        float4 SplatAlpha = g_SplatAlpha.Sample(g_BaseSmp, float3(input.UV, i));
        
        BaseTextureColor.rgb = BaseTextureColor.rgb * (1.f - SplatAlpha.r) + SplatDiffuse.rgb * SplatAlpha.r;
    }
    
    
    output.Diffuse.rgb = BaseTextureColor.rgb; // * (LightInfo.Dif + LightInfo.Amb) + LightInfo.Spc + LightInfo.Emv;

    output.Diffuse.a = BaseTextureColor.a * g_MtrlOpacity;
    
    output.GBuffer1.rgb = ComputeSplatBumpNormal(input.Normal, input.Tangent, input.Binormal, UV, input.UV);
    output.GBuffer1.a = 1.f;
    
    output.GBuffer2.r = input.ProjPos.z / input.ProjPos.w;
    output.GBuffer2.g = input.ProjPos.w;
    output.GBuffer2.b = g_MtrlSpecularColor.w;
    output.GBuffer2.a = 1.f;
    
    output.GBuffer3.r = ConvertColor(g_MtrlBaseColor);
    output.GBuffer3.g = ConvertColor(g_MtrlAmbientColor);
    
    output.GBuffer4.rgb = input.Tangent.xyz;
    output.GBuffer4.a = 1.f;
    
    output.GBuffer5.rgb = input.Binormal.xyz;
    output.GBuffer5.a = 1.f;
    
    float4 SpecularColor = g_MtrlSpecularColor.xyzw;
	
    if (g_MtrlSpecularTex)
        SpecularColor = g_SpecularTexture.Sample(g_BaseSmp, UV).rgba;
	
    for (int i = 0; i < g_LandScapeSplatCount; ++i)
    {
        float4 SplatSpecular = g_SplatSpecular.Sample(g_BaseSmp, float3(UV, i));
        
        float4 SplatAlpha = g_SplatAlpha.Sample(g_BaseSmp, float3(input.UV, i));
        
        SpecularColor.rgb = SpecularColor.rgb * (1.f - SplatAlpha.r) + SplatSpecular.rgb * SplatAlpha.r;
    }
    
    output.GBuffer3.b = ConvertColor(SpecularColor);
    
    float4 EmissiveColor = g_MtrlEmissiveColor.xyzw;
	
    if (g_MtrlEmissiveTex)
        EmissiveColor = g_EmissiveTexture.Sample(g_BaseSmp, UV).rgba;
    
    output.GBuffer3.a = ConvertColor(EmissiveColor);

    return output;

}
