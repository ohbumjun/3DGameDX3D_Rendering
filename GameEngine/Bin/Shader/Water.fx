
#include "TransparentInfo.fx"

struct Vertex3D
{
    // 변수 뒤에 : 레지스터이름 + 번호 로 지정한다.
    // 번호를 안붙이면 0으로 지정된다.
    float3 Pos : POSITION; // Vector3타입.
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    // float4 BlendWeight : BLENDWEIGHT;
    // float4 BlendIndex : BLENDINDEX;
};

struct Vertex3DOutput
{
    // SV가 붙으면 System Value이다. 이 값은 레지스터에 저장만 하고
    // 가져다 사용하면 안된다.
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD1;
    float4 ProjPos : POSITION;
    float3 ViewPos : POSITION1;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
};

struct PS_OUTPUT_Water
{
    float4 Diffuse : SV_Target;
    float4 GBuffer1 : SV_Target1;
    float4 GBuffer2 : SV_Target2;
    float4 GBuffer3 : SV_Target3;
};

cbuffer Water : register(b9)
{
    float g_WaveHeight;
    float g_WaveSpeed;
    float2 g_WaveFrequencey;
    float2 g_WaterEmpty;
};

Texture2DMS<float4> g_FinalRenderTarget : register(t21);

Vertex3DOutput WaterVS(Vertex3D input)
{
    Vertex3DOutput output = (Vertex3DOutput)0;

    float3 Pos = input.Pos;

    // 물 출령이는 효과 구현하기 
    float Wave = cos(g_AccTime * g_WaveSpeed + input.UV.x * g_WaveFrequencey.x);
    Wave += cos(g_AccTime * g_WaveSpeed + input.UV.y * g_WaveFrequencey.y);

    Pos.y += Wave * g_WaveHeight;

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

// PSOutput_GBuffer WaterPS(Vertex3DOutput input)
PSOutput_Single WaterPS(Vertex3DOutput input)
{
    PSOutput_Single output = (PSOutput_Single)0;

    // Sceen Space 로 전환하기 
    float2 ScreenUV = input.ProjPos.xy / input.ProjPos.w;

    ScreenUV = ScreenUV * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

    int2 TargetPos = (int2) 0;

    TargetPos.x = (int)(ScreenUV.x * g_Resolution.x);
    TargetPos.y = (int)(ScreenUV.y * g_Resolution.y);

    // 서로 어긋나는 잔파도 효과를 내기 위해 Normal 합하는 효과
    float2 UV1 = input.UV + float2(g_AccTime * 0.025f, 0.f);
    float2 UV2 = input.UV + float2(g_AccTime * 0.025f * -1.f, 0.f);
    float2 FinalUV = (UV1 + UV2) / 2.f;

    float3 NormalFirst = ComputeBumpNormal(input.Normal, input.Tangent, input.Binormal, UV1);
    float3 NormalSecond = ComputeBumpNormal(input.Normal, input.Tangent, input.Binormal, UV2);
    float3 FinalNormal = (NormalFirst + NormalSecond) * 0.5;

    // Sky Color 색상을 합하는 효과
    float3 IncomeViewDir = input.ViewPos;
    float3 ViewReflect = 2.f * FinalNormal * dot(FinalNormal, IncomeViewDir) - IncomeViewDir;
    ViewReflect = normalize(ViewReflect);

    // float3 ViewReflect = reflect(input.ViewPos, FinalNormal);

    float4 SkyReflectColor = g_SkyTex.Sample(g_BaseSmp, ViewReflect);

    float4 BaseTextureColor = g_BaseTexture.Sample(g_BaseSmp, input.UV.xy);
    BaseTextureColor.rgb *= g_MtrlBaseColor.rgb;

    /* 
    일반 공식 적용

    float4 FinalRenderedColor = g_FinalRenderTarget.Load(TargetPos, 0);

    float4 WaterDiffuseColor = BaseTextureColor * 0.7f + FinalRenderedColor * 0.3f;

    float4 SpecularColor = g_MtrlSpecularColor.xyzw;

    if (g_MtrlSpecularTex)
        SpecularColor = g_SpecularTexture.Sample(g_BaseSmp, input.UV.xy).rgba;

    float4 EmissiveColor = g_MtrlEmissiveColor.xyzw;

    if (g_MtrlEmissiveTex)
        EmissiveColor = g_EmissiveTexture.Sample(g_BaseSmp, input.UV.xy).rgba;

    // EmissiveColor = EmissiveColor * SkyReflectColor;
    EmissiveColor = SkyReflectColor;

    output.Color.rgb = BaseTextureColor.rgb + g_MtrlAmbientColor.rgb + SpecularColor.rgb + EmissiveColor.rgb;
    */

    /* Foward 적용하기 
    */
    LightInfo Info;
    LightResult LAcc;
    LightResult LResult;

    for (int i = 0; i < g_ForwardLightCount; ++i)
    {
        Info = g_LightInfoArray[i];

        // 각 Light 에 대한 결과값 얻어오기
        LResult = ComputeLightFromStructuredBuffer(Info, input.ViewPos, FinalNormal, input.UV.xy); // FinalUV 는 Normal 추출용 Only

        // 누적 결과
        LAcc.Dif += LResult.Dif;
        LAcc.Amb += LResult.Amb;
        LAcc.Spc += LResult.Spc;
        LAcc.Emv += LResult.Emv;

        if (i == 0)
            break;
    }


    // BaseTextureColor.rgb 색상도     ComputeLightFromStructuredBuffer 에서 같이 계산할 것이다.
   // output.Color.rgb = BaseTextureColor.rgb * LAcc.Dif.rgb + LAcc.Amb.rgb + LAcc.Spc.rgb + LAcc.Emv.rgb;
    output.Color.rgb = BaseTextureColor.rgb * LAcc.Dif.rgb;

    // Rim
    // Frenel => 시야 방향 벡터와, 대상의 노멀 방향 벡터의 관계 이용
    // 두 벡터가 이루는 각이 수직에 가까울 수록, 투과율이 감소하고
    // 반사율이 증가한다.
    // float Rim = saturate(dot());

    float rim = saturate(dot(normalize(FinalNormal), input.ViewPos * -1.f));
    float fresnel = pow(1 - rim, 3.f);
    // float fresnel = 1 - pow(rim, 2) * 0.95f;

 

    // 알파값 세팅
    // float4 BaseTextureColor = g_BaseTexture.Sample(g_BaseSmp, input.UV.xy);
    // BaseTextureColor.rgb = BaseTextureColor.rgb * g_MtrlBaseColor.rgb;

    // output.Color.a = BaseTextureColor.a * g_MtrlOpacity;
    output.Color.a = 1.f;

    return output;
}

/*

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
    float2 UV : TEXCOORD1;
    float4 ProjPos : POSITION;
    float3 ViewPos : POSITION1;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
};

struct PS_OUTPUT_Water
{
    float4 Diffuse : SV_Target;
    float4 GBuffer1 : SV_Target1;
    float4 GBuffer2 : SV_Target2;
    float4 GBuffer3 : SV_Target3;
};

cbuffer Water : register(b9)
{
    float g_WaveHeight;
    float g_WaveSpeed;
    float2 g_WaveFrequencey;
    float2 g_WaterEmpty;
};

Texture2DMS<float4> g_WaterPrevDiffuse : register(t5);
Texture2DMS<float4> g_WaterPrevGBufferDepth : register(t6);
Texture2DMS<float4> g_WaterPrevGBufferColor : register(t6);

Vertex3DOutput WaterVS(Vertex3D input)
{
    Vertex3DOutput output = (Vertex3DOutput)0;

    // 새로 그려내기

    float3 Pos = input.Pos;

    float Wave = cos(g_AccTime * g_WaveSpeed + input.UV.x * g_WaveFrequencey);
    Wave += cos(g_AccTime * g_WaveSpeed + input.UV.y * g_WaveFrequencey);

    // float cosTime = 0.5 *
    Pos.y += Wave * g_WaveHeight;

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

PS_OUTPUT_Water WaterPS(Vertex3DOutput input)
{
    PS_OUTPUT_Water output = (PS_OUTPUT_Water)0;

    float2 ScreenUV = input.ProjPos.xy / input.ProjPos.w;
    ScreenUV = ScreenUV * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

    int2 TargetPos = (int2) 0;

    TargetPos.x = (int)(ScreenUV.x * g_Resolution.x);
    TargetPos.y = (int)(ScreenUV.y * g_Resolution.y);

    // Rim
    // Frenel => 시야 방향 벡터와, 대상의 노멀 방향 벡터의 관계 이용
    // 두 벡터가 이루는 각이 수직에 가까울 수록, 투과율이 감소하고
    // 반사율이 증가한다.
    // float Rim = saturate(dot());

    float2 UV1 = input.UV + float2(g_AccTime * 0.025f, 0.f);
    float2 UV2 = input.UV + float2(g_AccTime * 0.025f * -1.f, 0.f);

    float3 NormalFirst = ComputeBumpNormal(input.Normal, input.Tangent, input.Binormal, UV1);
    float3 NormalSecond = ComputeBumpNormal(input.Normal, input.Tangent, input.Binormal, UV2);
    float3 FinalNormal = (NormalFirst + NormalSecond) * 0.5;

    float3 ViewReflect = reflect(FinalNormal, input.ViewPos);
    // float4 ProjReflect = normalize(mul(float4(ViewReflect, 1.f), g_matProj).xyz);
    // float4 ReflectColor = g_SkyTex.Sample(g_BaseSmp, ProjReflect);
    float4 ReflectColor = g_SkyTex.Sample(g_BaseSmp, ViewReflect);

    // 투과율 제어하기
   //  float penet = pow(dot(input.Normal, input.ViewPos), 5) * 0.2;

    // Fresnel -> 시야 방향 벡터와 노멀 방향 벡터의 관계를 이용한 공식
    // - 두 벡터가 이루는 각이 수직에 가까울 수록, 투과율이 감소하고, 반사율이 증가한다.
    float rim = saturate(dot(normalize(FinalNormal), input.ViewPos * -1.f));
    // float fresnel = pow(1 - rim, 0.05);
    float fresnel = 1 - pow(rim, 2) * 0.95f;

    float4 BaseTextureColor = g_BaseTexture.Sample(g_BaseSmp, input.UV.xy);

    float4 WaterDiffuseColor = BaseTextureColor + ReflectColor * (fresnel);

    float4 ExistingDiffuseColor = g_WaterPrevDiffuse.Load(TargetPos, 0);

    float4 Refraction = g_WaterPrevDiffuse.Load(TargetPos + FinalNormal.xy * 0.5f, 0) * 0.5f;

    // float4 FinalWaterColor = lerp(DiffuseColor, Refraction, pow(dot(FinalNormal, input.ViewPos * -1.f), 3));
    float4 FinalWaterColor = ExistingDiffuseColor * (1 - fresnel) + WaterDiffuseColor * (fresnel);

    float4 PrevDepth = g_WaterPrevGBufferDepth.Load(TargetPos, 0);
    float4 PrevColor = g_WaterPrevGBufferColor.Load(TargetPos, 0);

    // 두 벡터가 수직을 이룰 수록, 즉, 비스듬하게 보면, ReflectColor 를 더 많이 비추고
    // 물을 Direct 로 보면, 물 고유 색상을 더 잘 보이게 세팅
    // output.Diffuse.rgb = BaseTextureColor.rgb * (1 - fresnel) + ReflectColor.rgb * fresnel; // * (LightInfo.Dif + LightInfo.Amb) + LightInfo.Spc + LightInfo.Emv;
    // output.Diffuse.rgb = BaseTextureColor.rgb * (fresnel + 0.5f) + ReflectColor.rgb * (1 - fresnel); // * (LightInfo.Dif + LightInfo.Amb) + LightInfo.Spc + LightInfo.Emv;
    output.Diffuse.rgb = FinalWaterColor.rgb; // * (LightInfo.Dif + LightInfo.Amb) + LightInfo.Spc + LightInfo.Emv;
    // output.Diffuse.rgb = ReflectColor.rgb; // * (LightInfo.Dif + LightInfo.Amb) + LightInfo.Spc + LightInfo.Emv;
    // output.Diffuse.a = fresnel - penet;
    output.Diffuse.a = FinalWaterColor.a;

    output.GBuffer1.rgb = FinalNormal;
    output.GBuffer1.a = 1.f;

    output.GBuffer2.r = input.ProjPos.z / input.ProjPos.w;
    output.GBuffer2.g = input.ProjPos.w;
    output.GBuffer2.b = g_MtrlSpecularColor.w;
    output.GBuffer2.a = 1.f;

    output.GBuffer3.r = ConvertColor(g_MtrlBaseColor);
    output.GBuffer3.g = ConvertColor(g_MtrlAmbientColor);

    float4 SpecularColor = g_MtrlSpecularColor.xyzw;

    if (g_MtrlSpecularTex)
        SpecularColor = g_SpecularTexture.Sample(g_BaseSmp, input.UV.xy).rgba;

    output.GBuffer3.b = ConvertColor(SpecularColor);

    float4 EmissiveColor = g_MtrlEmissiveColor.xyzw;

    if (g_MtrlEmissiveTex)
        EmissiveColor = g_EmissiveTexture.Sample(g_BaseSmp, input.UV.xy).rgba;

    // output.GBuffer3.a = ConvertColor(ReflectColor * 0.6f * fresnel);
    // output.GBuffer3.a = ConvertColor(EmissiveColor);
    // output.GBuffer3.a = ConvertColor(EmissiveColor + ReflectColor * fresnel * 0.05f);
    output.GBuffer3.a = ConvertColor(PrevColor.a);

    return output;

}

*/