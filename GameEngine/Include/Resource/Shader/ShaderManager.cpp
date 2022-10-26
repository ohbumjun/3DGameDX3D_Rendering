// Shader
#include "ShaderManager.h"
#include "ColorMeshShader.h"
#include "Mesh2DShader.h"
#include "PosMeshShader.h"
#include "ColliderShader.h"
#include "ColliderPixelShader.h"
#include "WidgetShader.h"
#include "ProgressBarShader.h"
#include "NumberShader.h"
#include "ParticleUpdateShader.h"
#include "ParticleRenderShader.h"
#include "TileMapShader.h"
#include "Standard3DShader.h"
#include "AnimationUpdateShader.h"
#include "RenderTargetShader.h"
#include "LightAccShader.h"
#include "LightBlendShader.h"
#include "LightBlendRenderShader.h"
#include "SkyShader.h"
#include "LandScapeShader.h"
#include "DecalShader.h"
#include "WaterShader.h"
#include "DecalDebugShader.h"
#include "BillboardShader.h"
#include "Standard3DInstancingShader.h"
#include "ShadowMapShader.h"
#include "ShadowMapInstancingShader.h"
#include "HDRDownScaleFirstPass.h"

// Constant Buffer
#include "ShadowCBuffer.h"
#include "ConstantBuffer.h"
#include "LightFowardConstantBuffer.h"
#include "FirstHDRDownScaleCBuffer.h"
#include "HDRDownScaleSecondPass.h"


CShaderManager::CShaderManager()
{
}

CShaderManager::~CShaderManager()
{
}

bool CShaderManager::Init()
{
	if (!CreateShader<CColorMeshShader>("ColorMeshShader"))
		return false;
	if (!CreateShader<CPosMeshShader>("PosMeshShader"))
		return false;
	if (!CreateShader<CMesh2DShader>("Mesh2DShader"))
		return false;
	if (!CreateShader<CColliderShader>("ColliderShader"))
		return false;
	if (!CreateShader<CColliderPixelShader>("ColliderPixelShader"))
		return false;
	if (!CreateShader<CWidgetShader>("WidgetShader"))
		return false;
	if (!CreateShader<CProgressBarShader>("ProgressBarShader"))
		return false;
	if (!CreateShader<CNumberShader>("NumberShader"))
		return false;
	if (!CreateShader<CParticleUpdateShader>("ParticleUpdateShader"))
		return false;
	if (!CreateShader<CParticleRenderShader>("ParticleRenderShader"))
		return false;
	if (!CreateShader<CTileMapShader>("TileMapShader"))
		return false;
	if (!CreateShader<CStandard3DShader>("Standard3DShader"))
		return false;
	if (!CreateShader<CAnimationUpdateShader>("AnimationUpdateShader"))
		return false;
	if (!CreateShader<CRenderTargetShader>("RenderTargetShader"))
		return false;
	if (!CreateShader<CLightAccShader>("LightAccShader"))
		return false;
	if (!CreateShader<CLightBlendShader>("LightBlendShader"))
		return false;
	if (!CreateShader<CLightBlendRenderShader>("LightBlendRenderShader"))
		return false;
	if (!CreateShader<CSkyShader>("SkyShader"))
		return false;
	if (!CreateShader<CLandScapeShader>("LandScapeShader"))
		return false;
	if (!CreateShader<CDecalShader>("DecalShader"))
		return false;
	if (!CreateShader<CDecalDebugShader>("DecalDebugShader"))
		return false;
	if (!CreateShader<CBillboardShader>("BillboardShader"))
		return false;
	if (!CreateShader<CStandard3DInstancingShader>("Standard3DInstancingShader"))
		return false;
	if (!CreateShader<CShadowMapShader>("ShadowMapShader"))
		return false;
	if (!CreateShader<CShadowMapInstancingShader>("ShadowMapInstancingShader"))
		return false;
	if (!CreateShader<CWaterShader>("WaterShader"))
		return false;
	if (!CreateShader<CHDRDownScaleFirstPass>("HDRDownScaleFirstPass"))
		return false;
	if (!CreateShader<CHDRDownScaleSecondPass>("HDRDownScaleSecondPass"))
		return false;


	// =================== 상수버퍼 ===================
	CreateConstantBuffer("TransformCBuffer", sizeof(TransformCBuffer), 0,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("MaterialCBuffer", sizeof(MaterialCBuffer), 1,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("Standard2DCBuffer", sizeof(Standard2DCBuffer), 2,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("GlobalCBuffer", sizeof(GlobalCBuffer), 3,
		(int)Buffer_Shader_Type::All);

	CreateConstantBuffer("PaperBurnCBuffer", sizeof(PaperBurnCBuffer), 4,
		(int)Buffer_Shader_Type::Pixel);

	CreateConstantBuffer("LightCBuffer", sizeof(LightCBuffer), 5,
		(int)Buffer_Shader_Type::Pixel);

	CreateConstantBuffer("InstancingCBuffer", sizeof(InstancingCBuffer), 6,
		(int)Buffer_Shader_Type::Vertex | (int)Buffer_Shader_Type::Pixel);

	CreateConstantBuffer("Animation2DCBuffer", sizeof(Animation2DCBuffer), 10,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("ColliderCBuffer", sizeof(ColliderCBuffer), 11,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("WidgetCBuffer", sizeof(WidgetCBuffer), 11,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("ProgressBarCBuffer", sizeof(ProgressBarCBuffer), 12,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("ParticleCBuffer", sizeof(ParticleCBuffer), 11,
		(int)Buffer_Shader_Type::Compute);

	CreateConstantBuffer("TileMapCBuffer", sizeof(TileMapCBuffer), 11,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("AnimationCBuffer", sizeof(AnimationCBuffer), 11,
		(int)Buffer_Shader_Type::Compute);

	CreateConstantBuffer("LandScapeCBuffer", sizeof(LandScapeCBuffer), 11,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("ShadowCBuffer", sizeof(ShadowCBuffer), 10,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("WaterCBuffer", sizeof(WaterCBuffer), 9,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("LightFowardCBuffer", sizeof(LightFowardCBuffer), 8,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("FirstHDRDownScaleCBuffer", sizeof(FirstHDRDownScaleCBuffer), 7,
		(int)Buffer_Shader_Type::Graphic);

	

	return true;
}

CShader* CShaderManager::FindShader(const std::string& Name)
{
	auto	iter = m_mapShader.find(Name);

	if (iter == m_mapShader.end())
		return nullptr;

	return iter->second;
}

void CShaderManager::ReleaseShader(const std::string& Name)
{
	auto	iter = m_mapShader.find(Name);

	if (iter != m_mapShader.end())
	{
		if (iter->second->GetRefCount() == 1)
			m_mapShader.erase(iter);
	}
}

bool CShaderManager::CreateConstantBuffer(const std::string& Name, int Size, int Register, 
	int ConstantBufferShaderType)
{
	CConstantBuffer* Buffer = FindConstantBuffer(Name);

	if (Buffer)
		return true;

	Buffer = new CConstantBuffer;

	Buffer->SetName(Name);

	if (!Buffer->Init(Size, Register, ConstantBufferShaderType))
	{
		SAFE_RELEASE(Buffer);
		return false;
	}

	m_mapConstantBuffer.insert(std::make_pair(Name, Buffer));

	return true;
}

CConstantBuffer* CShaderManager::FindConstantBuffer(const std::string& Name)
{
	auto	iter = m_mapConstantBuffer.find(Name);

	if (iter == m_mapConstantBuffer.end())
		return nullptr;

	return iter->second;
}
