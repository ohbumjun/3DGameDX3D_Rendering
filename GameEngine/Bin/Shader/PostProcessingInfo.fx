#include "ShaderInfo.fx"

// Final Target 을 세팅해줄 것이다.
Texture2DMS<float4> HDRTex : register(t81);

// 휘도 계산을 위한 상수 
static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);