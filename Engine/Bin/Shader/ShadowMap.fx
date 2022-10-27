
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
    float4 ProjPos : POSITION;
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
    float4 ProjPos : POSITION;
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

SkinningInfo SkinningShadow(float3 Pos, float4 BlendWeight, float4 BlendIndex)
{
    SkinningInfo Info = (SkinningInfo) 0;
    
    if (g_MtrlAnimation3DEnable == 0)
    {
        Info.Pos = Pos;
        
        return Info;
    }
    
    for (int i = 0; i < 4; ++i)
    {
        if (BlendWeight[i] == 0.f)
            continue;
        
        matrix matBone = g_SkinningBoneMatrixArray[(int) BlendIndex[i]];
        
        Info.Pos += (mul(float4(Pos, 1.f), matBone) * BlendWeight[i]).xyz;
    }    
    
    return Info;

}


Vertex3DOutput Standard3DShadowMapVS(Vertex3D input)
{
    Vertex3DOutput output = (Vertex3DOutput) 0;

    SkinningInfo Info = SkinningShadow(input.Pos, input.BlendWeight, input.BlendIndex);
    
    
    float3 Pos = Info.Pos;

    output.ProjPos = mul(float4(Pos, 1.f), g_matWVP);
    output.Pos = output.ProjPos;

    return output;
}


PSOutput_Single Standard3DShadowMapPS(Vertex3DOutput input)
{
    PSOutput_Single output = (PSOutput_Single) 0;
    
    output.Color = (float4) (input.ProjPos.z / input.ProjPos.w);
    output.Color.w = input.ProjPos.w;

    return output;
}




SkinningInfo SkinningShadowInstancing(float3 Pos, 
    float4 BlendWeight, float4 BlendIndex, uint InstanceID)
{
    SkinningInfo Info = (SkinningInfo) 0;
    
    if (g_InstancingInfoArray[InstanceID].g_MtrlAnimation3DEnable == 0)
    {
        Info.Pos = Pos;
        
        return Info;
    }
    
    for (int i = 0; i < 4; ++i)
    {
        if (BlendWeight[i] == 0.f)
            continue;
        
        matrix matBone = g_SkinningBoneMatrixArray[InstanceID * g_InstancingBoneCount + (int) BlendIndex[i]];
        
        Info.Pos += (mul(float4(Pos, 1.f), matBone) * BlendWeight[i]).xyz;
    }    
    
    return Info;

}

Vertex3DOutputInstancing Standard3DShadowInstancingVS(Vertex3DInstancing input)
{
    Vertex3DOutputInstancing output = (Vertex3DOutputInstancing) 0;
    
    SkinningInfo Info = SkinningShadowInstancing(input.Pos, 
                    input.BlendWeight, input.BlendIndex, input.InstanceID);
    
    float3 Pos = Info.Pos;

    output.ProjPos = mul(float4(Pos, 1.f), g_InstancingInfoArray[input.InstanceID].matWVP);
    output.Pos = output.ProjPos;

    return output;
}

PSOutput_Single Standard3DShadowInstancingPS(Vertex3DOutputInstancing input)
{
    PSOutput_Single output = (PSOutput_Single) 0;
    
    output.Color = (float4) (input.ProjPos.z / input.ProjPos.w);
    output.Color.w = input.ProjPos.w;

    return output;
}
