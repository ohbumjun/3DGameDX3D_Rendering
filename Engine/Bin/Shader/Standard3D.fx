
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


struct Vertex3DInstancing
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
    uint InstanceID : SV_InstanceID;
};

struct Vertex3DOutputInstancing
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
    uint InstanceID : TEXCOORD1;
};



struct InstancingInfo
{
    matrix matWV;
    matrix matWVP;
    float4 g_MtrlBaseColor;
    float4 g_MtrlAmbientColor;
    float4 g_MtrlSpecularColor;
    float4 g_MtrlEmissiveColor;
    float g_MtrlOpacity;
    int g_MtrlPaperBurnEnable;
    int g_MtrlBumpEnable;
    int g_MtrlAnimation3DEnable;
    int g_MtrlSpecularTex;
    int g_MtrlEmissiveTex;
    int g_MtrlReceiveDecal;
    float g_PaperBurnFilter;
    int g_PaperBurnInverse;
    float g_PaperBurnInFilter;
    float g_PaperBurnOutFilter;
    float g_PaperBurnCenterFilter; 
    float4 g_PaperBurnInLineColor;
    float4 g_PaperBurnOutLineColor;
    float4 g_PaperBurnCenterLineColor;
};

StructuredBuffer<InstancingInfo> g_InstancingInfoArray : register(t40);


StructuredBuffer<matrix> g_SkinningBoneMatrixArray : register(t106);

struct SkinningInfo
{
    float3 Pos;
    float3 Normal;
    float3 Tangent;
    float3 Binormal;
};

SkinningInfo Skinning(float3 Pos, float3 Normal, float3 Tangent, float3 Binormal,
    float4 BlendWeight, float4 BlendIndex)
{
    SkinningInfo Info = (SkinningInfo)0;
    
    if (g_MtrlAnimation3DEnable == 0)
    {
        Info.Pos = Pos;
        Info.Normal = Normal;
        Info.Tangent = Tangent;
        Info.Binormal = Binormal;
        
        return Info;
    }
    
    for (int i = 0; i < 4; ++i)
    {
        if (BlendWeight[i] == 0.f)
            continue;
        
        matrix matBone = g_SkinningBoneMatrixArray[(int)BlendIndex[i]];
        
        Info.Pos += (mul(float4(Pos, 1.f), matBone) * BlendWeight[i]).xyz;
        Info.Normal += (mul(float4(Normal, 0.f), matBone) * BlendWeight[i]).xyz;
        Info.Tangent += (mul(float4(Tangent, 0.f), matBone) * BlendWeight[i]).xyz;
        Info.Binormal += (mul(float4(Binormal, 0.f), matBone) * BlendWeight[i]).xyz;
    }
    
    Info.Normal = normalize(Info.Normal);
    Info.Tangent = normalize(Info.Tangent);
    Info.Binormal = normalize(Info.Binormal);
    
    
    return Info;

}

/*
11 12 13 14
21 22 23 24
31 32 33 34
41 42 43 44

원근투영
34 : 뷰공간의 z값

이 말은 투영변환까지 거치면 나오는
x, y, z, w 에서 w에는
뷰공간의 z값이 들어간다.

내부적으로 최종 화면에서의 위치를
구할때
투영의 x, y, z, w가 있으면 이 값을 모두 w로 나누어준다.

x / w, y / w, z / w, w / w
*/


Vertex3DOutput Standard3DVS(Vertex3D input)
{
    Vertex3DOutput output = (Vertex3DOutput) 0;

    SkinningInfo Info = Skinning(input.Pos, input.Normal, input.Tangent,
                input.Binormal, input.BlendWeight, input.BlendIndex);
    
    
    float3 Pos = Info.Pos;

    output.ProjPos = mul(float4(Pos, 1.f), g_matWVP);
    output.Pos = output.ProjPos;
    
    // 뷰공간의 위치를 만들어준다.
    output.ViewPos = mul(float4(Pos, 1.f), g_matWV).xyz;
    
    // 뷰 공간의 Normal을 만들어준다.
    output.Normal = normalize(mul(float4(Info.Normal, 0.f), g_matWV).xyz);
    // 뷰 공간의 Tangent을 만들어준다.
    output.Tangent = normalize(mul(float4(Info.Tangent, 0.f), g_matWV).xyz);
    // 뷰 공간의 Binormal을 만들어준다.
    output.Binormal = normalize(mul(float4(Info.Binormal, 0.f), g_matWV).xyz);
    output.UV = input.UV;

    return output;
}


PSOutput_GBuffer Standard3DPS(Vertex3DOutput input)
{
    PSOutput_GBuffer output = (PSOutput_GBuffer) 0;
    
    //LightResult LightInfo = ComputeLight(input.ViewPos, input.Normal,
    //    input.Tangent, input.Binormal, input.UV);

    float4 BaseTextureColor = g_BaseTexture.Sample(g_BaseSmp, input.UV);

    if (BaseTextureColor.a == 0.f || g_MtrlOpacity == 0.f)
        clip(-1);
    
    output.Diffuse.rgb = BaseTextureColor.rgb;// * (LightInfo.Dif + LightInfo.Amb) + LightInfo.Spc + LightInfo.Emv;

    output.Diffuse.a = BaseTextureColor.a * g_MtrlOpacity;
    
    output.GBuffer1.rgb = ComputeBumpNormal(input.Normal, input.Tangent, input.Binormal, input.UV);
    output.GBuffer1.a = 1.f;
    
    output.GBuffer2.r = input.ProjPos.z / input.ProjPos.w;
    output.GBuffer2.g = input.ProjPos.w;
    output.GBuffer2.b = g_MtrlSpecularColor.w;
    output.GBuffer2.a = 1.f;
    
    output.GBuffer3.r = ConvertColor(g_MtrlBaseColor);
    output.GBuffer3.g = ConvertColor(g_MtrlAmbientColor);
    
    output.GBuffer4.rgb = input.Tangent.xyz;
    output.GBuffer4.a = g_MtrlReceiveDecal;
    
    output.GBuffer5.rgb = input.Binormal.xyz;
    output.GBuffer5.a = 1.f;
    
    float4 SpecularColor = g_MtrlSpecularColor.xyzw;
	
    if (g_MtrlSpecularTex)
        SpecularColor = g_SpecularTexture.Sample(g_BaseSmp, input.UV).xxxx;
	
    SpecularColor = float4(0.5f, 0.5f, 0.5f, 1.f);
    
    output.GBuffer3.b = ConvertColor(SpecularColor);
    
    float4 EmissiveColor = g_MtrlEmissiveColor.xyzw;
	
    if (g_MtrlEmissiveTex)
        EmissiveColor = g_EmissiveTexture.Sample(g_BaseSmp, input.UV).xxxx;
    
    output.GBuffer3.a = ConvertColor(EmissiveColor);

    return output;
}




SkinningInfo SkinningInstancing(float3 Pos, float3 Normal, float3 Tangent, float3 Binormal,
    float4 BlendWeight, float4 BlendIndex, uint InstanceID)
{
    SkinningInfo Info = (SkinningInfo) 0;
    
    if (g_InstancingInfoArray[InstanceID].g_MtrlAnimation3DEnable == 0)
    {
        Info.Pos = Pos;
        Info.Normal = Normal;
        Info.Tangent = Tangent;
        Info.Binormal = Binormal;
        
        return Info;
    }
    
    for (int i = 0; i < 4; ++i)
    {
        if (BlendWeight[i] == 0.f)
            continue;
        
        matrix matBone = g_SkinningBoneMatrixArray[InstanceID * g_InstancingBoneCount + (int) BlendIndex[i]];
        
        Info.Pos += (mul(float4(Pos, 1.f), matBone) * BlendWeight[i]).xyz;
        Info.Normal += (mul(float4(Normal, 0.f), matBone) * BlendWeight[i]).xyz;
        Info.Tangent += (mul(float4(Tangent, 0.f), matBone) * BlendWeight[i]).xyz;
        Info.Binormal += (mul(float4(Binormal, 0.f), matBone) * BlendWeight[i]).xyz;
    }
    
    Info.Normal = normalize(Info.Normal);
    Info.Tangent = normalize(Info.Tangent);
    Info.Binormal = normalize(Info.Binormal);
    
    
    return Info;

}

Vertex3DOutputInstancing Standard3DInstancingVS(Vertex3DInstancing input)
{
    Vertex3DOutputInstancing output = (Vertex3DOutputInstancing) 0;
    
    SkinningInfo Info = SkinningInstancing(input.Pos, input.Normal, input.Tangent,
                input.Binormal, input.BlendWeight, input.BlendIndex, input.InstanceID);
    
    output.InstanceID = input.InstanceID;
    
    float3 Pos = Info.Pos;

    output.ProjPos = mul(float4(Pos, 1.f), g_InstancingInfoArray[input.InstanceID].matWVP);
    output.Pos = output.ProjPos;
    
    // 뷰공간의 위치를 만들어준다.
    output.ViewPos = mul(float4(Pos, 1.f), g_InstancingInfoArray[input.InstanceID].matWV).xyz;
    
    // 뷰 공간의 Normal을 만들어준다.
    output.Normal = normalize(mul(float4(Info.Normal, 0.f), g_InstancingInfoArray[input.InstanceID].matWV).xyz);
    // 뷰 공간의 Tangent을 만들어준다.
    output.Tangent = normalize(mul(float4(Info.Tangent, 0.f), g_InstancingInfoArray[input.InstanceID].matWV).xyz);
    // 뷰 공간의 Binormal을 만들어준다.
    output.Binormal = normalize(mul(float4(Info.Binormal, 0.f), g_InstancingInfoArray[input.InstanceID].matWV).xyz);
    output.UV = input.UV;

    return output;
}


float3 ComputeBumpNormalInstancing(float3 Normal, float3 Tangent, float3 Binormal,
	float2 UV, uint InstanceID)
{
    float3 result = Normal;
	
    if (g_InstancingInfoArray[InstanceID].g_MtrlBumpEnable == 1)
    {
        float4 NormalColor = g_NormalTexture.Sample(g_BaseSmp, UV);
		
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


PSOutput_GBuffer Standard3DInstancingPS(Vertex3DOutputInstancing input)
{
    PSOutput_GBuffer output = (PSOutput_GBuffer) 0;
    
    //LightResult LightInfo = ComputeLight(input.ViewPos, input.Normal,
    //    input.Tangent, input.Binormal, input.UV);

    float4 BaseTextureColor = g_BaseTexture.Sample(g_BaseSmp, input.UV);

    if (BaseTextureColor.a == 0.f || g_InstancingInfoArray[input.InstanceID].g_MtrlOpacity == 0.f)
        clip(-1);
    
    output.Diffuse.rgb = BaseTextureColor.rgb; // * (LightInfo.Dif + LightInfo.Amb) + LightInfo.Spc + LightInfo.Emv;

    output.Diffuse.a = BaseTextureColor.a * g_InstancingInfoArray[input.InstanceID].g_MtrlOpacity;
    
    output.GBuffer1.rgb = ComputeBumpNormalInstancing(input.Normal, input.Tangent, input.Binormal, input.UV, input.InstanceID);
    output.GBuffer1.a = 1.f;
    
    output.GBuffer2.r = input.ProjPos.z / input.ProjPos.w;
    output.GBuffer2.g = input.ProjPos.w;
    output.GBuffer2.b = g_InstancingInfoArray[input.InstanceID].g_MtrlSpecularColor.w;
    output.GBuffer2.a = 1.f;
    
    output.GBuffer3.r = ConvertColor(g_InstancingInfoArray[input.InstanceID].g_MtrlBaseColor);
    output.GBuffer3.g = ConvertColor(g_InstancingInfoArray[input.InstanceID].g_MtrlAmbientColor);
    
    output.GBuffer4.rgb = input.Tangent.xyz;
    output.GBuffer4.a = g_InstancingInfoArray[input.InstanceID].g_MtrlReceiveDecal;
    
    output.GBuffer5.rgb = input.Binormal.xyz;
    output.GBuffer5.a = 1.f;
    
    float4 SpecularColor = g_InstancingInfoArray[input.InstanceID].g_MtrlSpecularColor.xyzw;
	
    if (g_InstancingInfoArray[input.InstanceID].g_MtrlSpecularTex)
        SpecularColor = g_SpecularTexture.Sample(g_BaseSmp, input.UV).xxxx;
	
    SpecularColor = float4(0.5f, 0.5f, 0.5f, 1.f);
    
    output.GBuffer3.b = ConvertColor(SpecularColor);
    
    float4 EmissiveColor = g_InstancingInfoArray[input.InstanceID].g_MtrlEmissiveColor.xyzw;
	
    if (g_InstancingInfoArray[input.InstanceID].g_MtrlEmissiveTex)
        EmissiveColor = g_EmissiveTexture.Sample(g_BaseSmp, input.UV).xxxx;
    
    output.GBuffer3.a = ConvertColor(EmissiveColor);

    return output;
}
