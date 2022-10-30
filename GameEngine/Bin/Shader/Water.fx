
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

// Texture2DMS<float4> g_FinalRenderTarget : register(t21);
TextureCube g_SkyReflectTex : register(t23);

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

    float4 SkyReflectColor = g_SkyReflectTex.Sample(g_BaseSmp, ViewReflect);

    float4 BaseTextureColor = g_BaseTexture.Sample(g_BaseSmp, input.UV.xy);
    BaseTextureColor.rgb *= g_MtrlBaseColor.rgb;
    BaseTextureColor.a *= g_MtrlOpacity;

    // Gamma Correction 적용 하기 (Texture 로부터 추출해온 색상이 sRGB 라고 가정 -> Linear Space 로 보내기)
    BaseTextureColor.rgb = pow(BaseTextureColor.rgb, 1 / 2.2f);
    SkyReflectColor.rgb   = pow(SkyReflectColor.rgb, 1 / 2.2f);

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

    // error X4000 방지 위한 것
    LAcc.Dif = float3(0.f, 0.f, 0.f); LAcc.Amb = float3(0.f, 0.f, 0.f); LAcc.Spc = float3(0.f, 0.f, 0.f); LAcc.Emv = float3(0.f, 0.f, 0.f);

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
    float3 FinalBaseColor = BaseTextureColor.rgb * 0.5f + SkyReflectColor;
    output.Color.rgb = FinalBaseColor * LAcc.Dif.rgb + LAcc.Amb.rgb + LAcc.Spc.rgb + LAcc.Emv.rgb;

    // 최종 출력 색상을 한단계 위로 올려준다. (Linear -> sRGB 상으로 올려주기)
    output.Color.rgb = pow(output.Color.rgb, 2.2f);

    // Rim
    // Frenel => 시야 방향 벡터와, 대상의 노멀 방향 벡터의 관계 이용
    // 두 벡터가 이루는 각이 수직에 가까울 수록, 투과율이 감소하고
    // 반사율이 증가한다.

    // float rim = saturate(dot(normalize(FinalNormal), normalize(input.ViewPos * -1.f)));
    float rim = saturate(dot(normalize(FinalNormal), normalize(input.ViewPos) * -1.f));
    float exponential = pow(1 - rim, 5);
    float fresnel = 0.3f + 0.7f * exponential;
    
    output.Color.a = fresnel;

    return output;
}
