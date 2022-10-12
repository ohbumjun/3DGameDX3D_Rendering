
#include "RenderManager.h"
#include "../GameObject/GameObject.h"
#include "../Component/StaticMeshComponent.h"
#include "../Component/AnimationMeshComponent.h"
#include "RenderStateManager.h"
#include "../Resource/Shader/Standard2DConstantBuffer.h"
#include "../Resource/ResourceManager.h"
#include "../Resource/Mesh/AnimationMesh.h"
#include "RenderState.h"
#include "../Scene/SceneManager.h"
#include "../Scene/Scene.h"
#include "../Scene/Viewport.h"
#include "../Engine.h"
#include "../Device.h"
#include "../Resource/Shader/Shader.h"
#include "../Resource/Shader/ConstantBuffer.h"
#include "../Component/PaperBurnComponent.h"
#include "../Resource/Shader/ShadowCBuffer.h"

DEFINITION_SINGLE(CRenderManager)

CRenderManager::CRenderManager()	:
	m_RenderStateManager(nullptr),
	m_Standard2DCBuffer(nullptr),
	m_Shadow(true),
	m_ShadowLightDistance(20.f),
	m_ShadowCBuffer(nullptr),
	m_ShadowBias(0.00307193305f)
{
}

CRenderManager::~CRenderManager()
{
	SAFE_DELETE(m_ShadowCBuffer);

	auto	iter = m_RenderLayerList.begin();
	auto	iterEnd = m_RenderLayerList.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE((*iter));
	}

	m_RenderLayerList.clear();

	SAFE_DELETE(m_Standard2DCBuffer);
	SAFE_DELETE(m_RenderStateManager);
}

void CRenderManager::AddRenderList(CSceneComponent* Component)
{
	RenderLayer* Layer = nullptr;

	auto	iter = m_RenderLayerList.begin();
	auto	iterEnd = m_RenderLayerList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->Name == Component->GetLayerName())
		{
			Layer = *iter;
			break;
		}
	}

	if (!Layer)
		return;

	Layer->RenderList.push_back(Component);
}

void CRenderManager::CreateLayer(const std::string& Name, int Priority)
{
	RenderLayer* Layer = new RenderLayer;
	Layer->Name = Name;
	Layer->LayerPriority = Priority;

	m_RenderLayerList.push_back(Layer);

	sort(m_RenderLayerList.begin(), m_RenderLayerList.end(), CRenderManager::Sortlayer);
}

void CRenderManager::SetLayerPriority(const std::string& Name, int Priority)
{
	auto	iter = m_RenderLayerList.begin();
	auto	iterEnd = m_RenderLayerList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->Name == Name)
		{
			(*iter)->LayerPriority = Priority;
			break;
		}
	}

	sort(m_RenderLayerList.begin(), m_RenderLayerList.end(), CRenderManager::Sortlayer);
}

bool CRenderManager::Init()
{
	m_RenderStateManager = new CRenderStateManager;

	m_RenderStateManager->Init();

	m_Standard2DCBuffer = new CStandard2DConstantBuffer;

	m_Standard2DCBuffer->Init();

	// 기본 레이어 생성
	RenderLayer* Layer = new RenderLayer;
	Layer->Name = "Back";
	Layer->LayerPriority = (int)RenderLayerIdx::Back;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "Default";
	Layer->LayerPriority = (int)RenderLayerIdx::Default;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "Decal";
	Layer->LayerPriority = (int)RenderLayerIdx::Decal;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "Particle";
	Layer->LayerPriority = (int)RenderLayerIdx::Particle;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "Transparent";
	Layer->LayerPriority = (int)RenderLayerIdx::Transparent;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "ScreenWidgetComponent";
	Layer->LayerPriority = (int)RenderLayerIdx::ScreenWidgetComponent;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "DebugLayer";
	Layer->LayerPriority = (int)RenderLayerIdx::DebugLayer;

	m_RenderLayerList.push_back(Layer);

	// Culling, Picking Layer
	Layer = new RenderLayer;
	Layer->Name = "PickingCullingLayer";
	Layer->LayerPriority = (int)RenderLayerIdx::PickingCullingLayer;

	// Transparent
	Layer = new RenderLayer;
	Layer->Name = "Transparent";
	Layer->LayerPriority = 7;

	m_RenderLayerList.push_back(Layer);

	m_DepthDisable = m_RenderStateManager->FindRenderState("DepthDisable");
	m_AlphaBlend = m_RenderStateManager->FindRenderState("AlphaBlend");
	m_LightAccBlend = m_RenderStateManager->FindRenderState("LightAcc");

	// 디퍼드 렌더링용 타겟 생성
	Resolution RS = CDevice::GetInst()->GetResolution();

	if (!CResourceManager::GetInst()->CreateTarget("Diffuse",
		RS.Width, RS.Height, DXGI_FORMAT_R8G8B8A8_UNORM))
		return false;

	if (!CResourceManager::GetInst()->CreateTarget("GBuffer1",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	if (!CResourceManager::GetInst()->CreateTarget("GBuffer2",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	if (!CResourceManager::GetInst()->CreateTarget("GBuffer3",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	if (!CResourceManager::GetInst()->CreateTarget("GBuffer4",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	if (!CResourceManager::GetInst()->CreateTarget("GBuffer5",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	CRenderTarget* GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("Diffuse");
	GBufferTarget->SetPos(Vector3(0.f, 0.f, 0.f));
	GBufferTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	GBufferTarget->SetDebugRender(true);
	m_vecGBuffer.push_back(GBufferTarget);

	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer1");
	GBufferTarget->SetPos(Vector3(0.f, 100.f, 0.f));
	GBufferTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	GBufferTarget->SetDebugRender(true);
	m_vecGBuffer.push_back(GBufferTarget);

	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer2");
	GBufferTarget->SetPos(Vector3(0.f, 200.f, 0.f));
	GBufferTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	GBufferTarget->SetDebugRender(true);
	m_vecGBuffer.push_back(GBufferTarget);

	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer3");
	GBufferTarget->SetPos(Vector3(0.f, 300.f, 0.f));
	GBufferTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	GBufferTarget->SetDebugRender(true);
	m_vecGBuffer.push_back(GBufferTarget);

	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer4");
	GBufferTarget->SetPos(Vector3(0.f, 400.f, 0.f));
	GBufferTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	GBufferTarget->SetDebugRender(true);
	m_vecGBuffer.push_back(GBufferTarget);

	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer5");
	GBufferTarget->SetPos(Vector3(0.f, 500.f, 0.f));
	GBufferTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	GBufferTarget->SetDebugRender(true);
	m_vecGBuffer.push_back(GBufferTarget);



	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("Diffuse");
	m_vecDecal.push_back(GBufferTarget);

	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer1");
	m_vecDecal.push_back(GBufferTarget);

	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer3");
	m_vecDecal.push_back(GBufferTarget);



	if (!CResourceManager::GetInst()->CreateTarget("LightDif",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	if (!CResourceManager::GetInst()->CreateTarget("LightSpc",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	if (!CResourceManager::GetInst()->CreateTarget("LightEmv",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	CRenderTarget* LightTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("LightDif");
	LightTarget->SetPos(Vector3(100.f, 0.f, 0.f));
	LightTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	LightTarget->SetDebugRender(true);
	m_vecLightBuffer.push_back(LightTarget);

	LightTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("LightSpc");
	LightTarget->SetPos(Vector3(100.f, 100.f, 0.f));
	LightTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	LightTarget->SetDebugRender(true);
	m_vecLightBuffer.push_back(LightTarget);

	LightTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("LightEmv");
	LightTarget->SetPos(Vector3(100.f, 200.f, 0.f));
	LightTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	LightTarget->SetDebugRender(true);
	m_vecLightBuffer.push_back(LightTarget);



	if (!CResourceManager::GetInst()->CreateTarget("FinalScreen",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	CRenderTarget* FinalScreenTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("FinalScreen");
	FinalScreenTarget->SetPos(Vector3(200.f, 0.f, 0.f));
	FinalScreenTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	FinalScreenTarget->SetDebugRender(true);


	if (!CResourceManager::GetInst()->CreateTarget("ShadowMap",
		(unsigned int)SHADOWMAP_WIDTH, (unsigned int)SHADOWMAP_HEIGHT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_D24_UNORM_S8_UINT))
		return false;

	m_ShadowMapTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("ShadowMap");
	m_ShadowMapTarget->SetPos(Vector3(300.f, 0.f, 0.f));
	m_ShadowMapTarget->SetScale(Vector3(300.f, 300.f, 1.f));
	m_ShadowMapTarget->SetDebugRender(true);

	m_LightBlendShader = CResourceManager::GetInst()->FindShader("LightBlendShader");
	m_LightBlendRenderShader = CResourceManager::GetInst()->FindShader("LightBlendRenderShader");
	m_Standard3DInstancingShader = CResourceManager::GetInst()->FindShader("Standard3DInstancingShader");
	m_ShadowMapShader = CResourceManager::GetInst()->FindShader("ShadowMapShader");
	m_ShadowMapInstancingShader = CResourceManager::GetInst()->FindShader("ShadowMapInstancingShader");

	m_ShadowCBuffer = new CShadowCBuffer;

	m_ShadowCBuffer->Init();

	return true;
}

void CRenderManager::Render(float DeltaTime)
{
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_ShadowBias -= DeltaTime * 0.2f;

		m_ShadowCBuffer->SetBias(m_ShadowBias);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_ShadowBias += DeltaTime * 0.2f;

		m_ShadowCBuffer->SetBias(m_ShadowBias);
	}

	{
		auto	iter = m_RenderLayerList.begin();
		auto	iterEnd = m_RenderLayerList.end();

		for (; iter != iterEnd; ++iter)
		{
			(*iter)->RenderList.clear();
			(*iter)->InstancingIndex = 0;

			auto	iter1 = (*iter)->m_vecInstancing.begin();
			auto	iter1End = (*iter)->m_vecInstancing.end();

			for (; iter1 != iter1End; ++iter1)
			{
				(*iter1)->RenderList.clear();
			}
		}
	}

	{
		auto	iter = m_ObjectList->begin();
		auto	iterEnd = m_ObjectList->end();

		for (; iter != iterEnd; ++iter)
		{
			(*iter)->PrevRender();
		}
	}


	{
		const std::list<InstancingCheckCount*>* InstancingList = CSceneComponent::GetInstancingCheckList();

		auto	iter = InstancingList->begin();
		auto	iterEnd = InstancingList->end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter)->InstancingList.size() >= 10)
			{
				RenderLayer* Layer = nullptr;

				size_t	Size = m_RenderLayerList.size();

				for (size_t i = 0; i < Size; ++i)
				{
					if (m_RenderLayerList[i]->Name == (*iter)->LayerName)
					{
						Layer = m_RenderLayerList[i];
						break;
					}
				}

				if (Layer)
				{
					if (Layer->m_vecInstancing.size() == Layer->InstancingIndex)
					{
						Layer->m_vecInstancing.resize(Layer->InstancingIndex * 2);

						for (int i = 0; i < Layer->InstancingIndex; ++i)
						{
							Layer->m_vecInstancing[Layer->InstancingIndex + i] = new RenderInstancingList;
						}
					}

					auto	iter1 = (*iter)->InstancingList.begin();
					auto	iter1End = (*iter)->InstancingList.end();

					if ((*iter)->InstancingList.size() > Layer->m_vecInstancing[Layer->InstancingIndex]->BufferCount)
					{
						int	Count = (int)(Layer->m_vecInstancing[Layer->InstancingIndex]->BufferCount * 1.5f);

						if ((*iter)->InstancingList.size() > Count)
							Count = (int)(*iter)->InstancingList.size();

						SAFE_DELETE(Layer->m_vecInstancing[Layer->InstancingIndex]->Buffer);
						SAFE_DELETE(Layer->m_vecInstancing[Layer->InstancingIndex]->ShadowBuffer);

						Layer->m_vecInstancing[Layer->InstancingIndex]->Buffer = new CStructuredBuffer;

						Layer->m_vecInstancing[Layer->InstancingIndex]->Buffer->Init("InstancingBuffer", sizeof(Instancing3DInfo),
							Count, 40, true,
							(int)Buffer_Shader_Type::Vertex | (int)Buffer_Shader_Type::Pixel);

						Layer->m_vecInstancing[Layer->InstancingIndex]->ShadowBuffer = new CStructuredBuffer;

						Layer->m_vecInstancing[Layer->InstancingIndex]->ShadowBuffer->Init("InstancingShadowBuffer", sizeof(Instancing3DInfo),
							Count, 40, true,
							(int)Buffer_Shader_Type::Vertex | (int)Buffer_Shader_Type::Pixel);
					}

					for (; iter1 != iter1End; ++iter1)
					{
						// 구조화 버퍼에 정보를 채울 수 있는 함수를 만들어서 정보를 채워준다.
						Layer->m_vecInstancing[Layer->InstancingIndex]->RenderList.push_back(*iter1);
					}

					Layer->m_vecInstancing[Layer->InstancingIndex]->Mesh = (*iter)->Mesh;

					if ((*iter)->Mesh->CheckType<CAnimationMesh>())
					{
						Layer->m_vecInstancing[Layer->InstancingIndex]->Animation = true;
						Layer->m_vecInstancing[Layer->InstancingIndex]->CBuffer->SetBoneCount(((CAnimationMesh*)(*iter)->Mesh)->GetBoneCount());
					}

					++Layer->InstancingIndex;
				}
			}
		}
	}

	// 환경맵 출력
	CSharedPtr<CGameObject>	SkyObj = CSceneManager::GetInst()->GetScene()->GetSkyObject();

	SkyObj->Render();

	// 인스턴싱 정보를 만든다.
	RenderDefaultInstancingInfo();

	// 그림자 맵을 만든다.
	RenderShadowMap();

	// GBuffer를 만들어낸다.
	RenderGBuffer();

	// Decal을 그려낸다.
	RenderDecal();

	// 트랜스폼 정보를 넘겨주어야 한다.
	TransformCBuffer	TBuffer = {};

	CScene* Scene = CSceneManager::GetInst()->GetScene();
	CCameraComponent* Camera = Scene->GetCameraManager()->GetCurrentCamera();

	TBuffer.matView = Camera->GetViewMatrix();
	TBuffer.matProj = Camera->GetProjMatrix();
	TBuffer.matInvProj = TBuffer.matProj;
	TBuffer.matInvProj.Inverse();
	TBuffer.matWV = TBuffer.matView;
	TBuffer.matWVP = TBuffer.matView * TBuffer.matProj;
	TBuffer.matVP = TBuffer.matWVP;
	TBuffer.matInvVP = TBuffer.matWVP;
	TBuffer.matInvVP.Inverse();
	TBuffer.matInvWVP = TBuffer.matWVP;
	TBuffer.matInvWVP.Inverse();

	TBuffer.matView.Transpose();
	TBuffer.matProj.Transpose();
	TBuffer.matInvProj.Transpose();
	TBuffer.matWV.Transpose();
	TBuffer.matWVP.Transpose();
	TBuffer.matVP.Transpose();
	TBuffer.matInvVP.Transpose();
	TBuffer.matInvWVP.Transpose();

	CResourceManager::GetInst()->FindConstantBuffer("TransformCBuffer")->UpdateBuffer(&TBuffer);

	// 조명 누적버퍼를 만들어낸다.
	RenderLightAcc();

	// 조명 누적버퍼와 GBuffer를 이용하여 최종화면을 만들어낸다.
	RenderLightBlend();

	// 조명처리된 최종 화면을 백버퍼에 그려낸다.
	RenderFinalScreen();

	// 반투명 물체를 그려낸다.
	RenderTransparent();

	m_vecGBuffer[2]->SetShader(10, (int)Buffer_Shader_Type::Pixel, 0);

	m_AlphaBlend->SetState();

	// 파티클 레이어 출력
	// auto	iter = m_RenderLayerList[3]->RenderList.begin();
	auto	iter = m_RenderLayerList[(int)RenderLayerIdx::Particle]->RenderList.begin();
	auto	iterEnd = m_RenderLayerList[(int)RenderLayerIdx::Particle]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Render();
	}

	// Collider 등 Debug 시 Light 적용과 별도로 Render 해줄 녀석들 출력하기 
	RenderColliderComponents();

	// Culling 시 활용되는 영역을 보여준다. (Sphere 단위이지만, 우선 임시적으로 정육면체 영역 형태로 보여주기)
	RenderCullingLayer();

	m_AlphaBlend->ResetState();

	m_vecGBuffer[2]->ResetShader(10, (int)Buffer_Shader_Type::Pixel, 0);

	// Screen Widget 출력
	// iterEnd = m_RenderLayerList[4]->RenderList.end();
	iter = m_RenderLayerList[(int)RenderLayerIdx::ScreenWidgetComponent]->RenderList.begin();
	iterEnd = m_RenderLayerList[(int)RenderLayerIdx::ScreenWidgetComponent]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Render();
	}

	// 조명 정보를 Shader로 넘겨준다.
	//CSceneManager::GetInst()->GetScene()->GetLightManager()->SetShader();

	/*{
		auto	iter = m_RenderLayerList.begin();
		auto	iterEnd = m_RenderLayerList.end();

		for (; iter != iterEnd; ++iter)
		{
			for (int i = 0; i < (*iter)->RenderCount; ++i)
			{
				(*iter)->RenderList[i]->Render();
			}
		}
	}*/

	{
		auto	iter1 = m_RenderLayerList.begin();
		auto	iter1End = m_RenderLayerList.end();

		for (; iter1 != iter1End; ++iter1)
		{
			auto	iter2 = (*iter1)->RenderList.begin();
			auto	iter2End = (*iter1)->RenderList.end();

			for (; iter2 != iter2End; ++iter2)
			{
				(*iter2)->PostRender();
			}
		}
	}


	// Widget 출력
	m_DepthDisable->SetState();

	m_AlphaBlend->SetState();

	CSceneManager::GetInst()->GetScene()->GetViewport()->Render();


	// 디버깅용 렌더타겟을 출력한다.
	CResourceManager::GetInst()->RenderTarget();



	// 마우스 출력
	CWidgetWindow* MouseWidget = CEngine::GetInst()->GetMouseWidget();

	if (MouseWidget)
		MouseWidget->Render();

	m_AlphaBlend->ResetState();

	m_DepthDisable->ResetState();
}

void CRenderManager::RenderShadowMap()
{
	D3D11_VIEWPORT	VP = {};

	VP.Width = SHADOWMAP_WIDTH;
	VP.Height = SHADOWMAP_HEIGHT;
	VP.MaxDepth = 1.f;

	CDevice::GetInst()->GetContext()->RSSetViewports(1, &VP);

	m_ShadowMapTarget->ClearTarget();
	m_ShadowMapTarget->SetTarget();

	m_ShadowMapShader->SetShader();

	for (size_t i = 0; i <= (int)RenderLayerIdx::Default; ++i)
	{
		auto	iter = m_RenderLayerList[i]->RenderList.begin();
		auto	iterEnd = m_RenderLayerList[i]->RenderList.end();

		for (; iter != iterEnd; ++iter)
		{
			(*iter)->RenderShadowMap();
		}
	}

	/*for (int i = 0; i < m_RenderLayerList[1]->InstancingIndex; ++i)
	{
		auto	iter = m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.begin();
		auto	iterEnd = m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.end();

		for (; iter != iterEnd; ++iter)
		{
			(*iter)->Render();
		}
	}*/

	RenderDefaultInstancingShadow();


	m_ShadowMapTarget->ResetTarget();

	D3D11_VIEWPORT	PrevVP = {};

	PrevVP.Width = (float)CDevice::GetInst()->GetResolution().Width;
	PrevVP.Height = (float)CDevice::GetInst()->GetResolution().Height;
	PrevVP.MaxDepth = 1.f;

	CDevice::GetInst()->GetContext()->RSSetViewports(1, &PrevVP);
}

void CRenderManager::RenderGBuffer()
{
	// 현재 백버퍼로 렌더타겟이 지정되어 있다.
	// 이를 GBuffer 렌더타겟으로 교체해야 한다.
	std::vector<ID3D11RenderTargetView*>	vecTarget;
	std::vector<ID3D11RenderTargetView*>	vecPrevTarget;
	ID3D11DepthStencilView* PrevDepthTarget = nullptr;

	size_t	GBufferSize = m_vecGBuffer.size();

	vecPrevTarget.resize(GBufferSize);

	for (size_t i = 0; i < GBufferSize; ++i)
	{
		m_vecGBuffer[i]->ClearTarget();
		vecTarget.push_back(m_vecGBuffer[i]->GetTargetView());
	}

	// 현재 지정되어 있는 렌더타겟과 깊이타겟을 얻어온다.
	CDevice::GetInst()->GetContext()->OMGetRenderTargets((unsigned int)GBufferSize,
		&vecPrevTarget[0], &PrevDepthTarget);

	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)GBufferSize,
		&vecTarget[0], PrevDepthTarget);

	for (size_t i = 0; i <= (int)RenderLayerIdx::Default; ++i)
	{
		auto	iter = m_RenderLayerList[i]->RenderList.begin();
		auto	iterEnd = m_RenderLayerList[i]->RenderList.end();

		for (; iter != iterEnd; ++iter)
		{
			(*iter)->Render();
		}
	}

	/*for (int i = 0; i < m_RenderLayerList[1]->InstancingIndex; ++i)
	{
		auto	iter = m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.begin();
		auto	iterEnd = m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.end();

		for (; iter != iterEnd; ++iter)
		{
			(*iter)->Render();
		}
	}*/
	
	RenderDefaultInstancing();


	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)GBufferSize,
		&vecPrevTarget[0], PrevDepthTarget);

	SAFE_RELEASE(PrevDepthTarget);
	for (size_t i = 0; i < GBufferSize; ++i)
	{
		SAFE_RELEASE(vecPrevTarget[i]);
	}
}

void CRenderManager::RenderDecal()
{
	// 현재 백버퍼로 렌더타겟이 지정되어 있다.
	// 이를 GBuffer 렌더타겟으로 교체해야 한다.
	std::vector<ID3D11RenderTargetView*>	vecTarget;
	std::vector<ID3D11RenderTargetView*>	vecPrevTarget;
	ID3D11DepthStencilView* PrevDepthTarget = nullptr;

	size_t	DecalSize = m_vecDecal.size();

	vecPrevTarget.resize(DecalSize);

	for (size_t i = 0; i < DecalSize; ++i)
	{
		vecTarget.push_back(m_vecDecal[i]->GetTargetView());
	}

	// 현재 지정되어 있는 렌더타겟과 깊이타겟을 얻어온다.
	CDevice::GetInst()->GetContext()->OMGetRenderTargets((unsigned int)DecalSize,
		&vecPrevTarget[0], &PrevDepthTarget);

	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)DecalSize,
		&vecTarget[0], PrevDepthTarget);


	m_vecGBuffer[2]->SetTargetShader(10);
	m_vecGBuffer[4]->SetTargetShader(11);
	m_vecGBuffer[5]->SetTargetShader(12);


	// auto	iter = m_RenderLayerList[2]->RenderList.begin();
	auto	iter = m_RenderLayerList[(int)RenderLayerIdx::Decal]->RenderList.begin();
	auto	iterEnd = m_RenderLayerList[(int)RenderLayerIdx::Decal]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Render();
	}


	m_vecGBuffer[2]->ResetTargetShader(10);
	m_vecGBuffer[4]->ResetTargetShader(11);
	m_vecGBuffer[5]->ResetTargetShader(12);


	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)DecalSize,
		&vecPrevTarget[0], PrevDepthTarget);

	SAFE_RELEASE(PrevDepthTarget);
	for (size_t i = 0; i < DecalSize; ++i)
	{
		SAFE_RELEASE(vecPrevTarget[i]);
	}



	// 디버깅 출력
#ifdef _DEBUG

	std::vector<ID3D11RenderTargetView*>	vecTarget1;
	std::vector<ID3D11RenderTargetView*>	vecPrevTarget1;
	ID3D11DepthStencilView* PrevDepthTarget1 = nullptr;

	size_t	GBufferSize = m_vecGBuffer.size();

	vecPrevTarget1.resize(GBufferSize);

	for (size_t i = 0; i < GBufferSize; ++i)
	{
		vecTarget1.push_back(m_vecGBuffer[i]->GetTargetView());
	}

	// 현재 지정되어 있는 렌더타겟과 깊이타겟을 얻어온다.
	CDevice::GetInst()->GetContext()->OMGetRenderTargets((unsigned int)GBufferSize,
		&vecPrevTarget1[0], &PrevDepthTarget1);

	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)GBufferSize,
		&vecTarget1[0], PrevDepthTarget1);


	// iter = m_RenderLayerList[2]->RenderList.begin();
	iter = m_RenderLayerList[(int)RenderLayerIdx::Decal]->RenderList.begin();
	iterEnd = m_RenderLayerList[(int)RenderLayerIdx::Decal]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->RenderDebug();
	}


	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)GBufferSize,
		&vecPrevTarget1[0], PrevDepthTarget1);

	SAFE_RELEASE(PrevDepthTarget1);
	for (size_t i = 0; i < GBufferSize; ++i)
	{
		SAFE_RELEASE(vecPrevTarget1[i]);
	}

#endif // _DEBUG

}

void CRenderManager::RenderLightAcc()
{
	std::vector<ID3D11RenderTargetView*>	vecTarget;
	std::vector<ID3D11RenderTargetView*>	vecPrevTarget;
	ID3D11DepthStencilView* PrevDepthTarget = nullptr;

	size_t	LightBufferSize = m_vecLightBuffer.size();

	vecPrevTarget.resize(LightBufferSize);

	for (size_t i = 0; i < LightBufferSize; ++i)
	{
		m_vecLightBuffer[i]->ClearTarget();
		vecTarget.push_back(m_vecLightBuffer[i]->GetTargetView());
	}

	// 현재 지정되어 있는 렌더타겟과 깊이타겟을 얻어온다.
	CDevice::GetInst()->GetContext()->OMGetRenderTargets((unsigned int)LightBufferSize,
		&vecPrevTarget[0], &PrevDepthTarget);

	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)LightBufferSize,
		&vecTarget[0], PrevDepthTarget);


	m_LightAccBlend->SetState();

	m_DepthDisable->SetState();


	m_vecGBuffer[0]->SetTargetShader(14);
	m_vecGBuffer[1]->SetTargetShader(15);
	m_vecGBuffer[2]->SetTargetShader(16);
	m_vecGBuffer[3]->SetTargetShader(17);

	CSceneManager::GetInst()->GetScene()->GetLightManager()->Render();

	m_vecGBuffer[0]->ResetTargetShader(14);
	m_vecGBuffer[1]->ResetTargetShader(15);
	m_vecGBuffer[2]->ResetTargetShader(16);
	m_vecGBuffer[3]->ResetTargetShader(17);

	m_DepthDisable->ResetState();

	m_LightAccBlend->ResetState();


	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)LightBufferSize,
		&vecPrevTarget[0], PrevDepthTarget);

	SAFE_RELEASE(PrevDepthTarget);
	for (size_t i = 0; i < LightBufferSize; ++i)
	{
		SAFE_RELEASE(vecPrevTarget[i]);
	}
}

void CRenderManager::RenderLightBlend()
{
	CRenderTarget* FinalScreenTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("FinalScreen");

	FinalScreenTarget->ClearTarget();

	FinalScreenTarget->SetTarget(nullptr);

	m_vecGBuffer[0]->SetTargetShader(14);
	m_vecGBuffer[2]->SetTargetShader(16);
	m_vecLightBuffer[0]->SetTargetShader(18);
	m_vecLightBuffer[1]->SetTargetShader(19);
	m_vecLightBuffer[2]->SetTargetShader(20);
	m_ShadowMapTarget->SetTargetShader(22);

	m_LightBlendShader->SetShader();

	m_DepthDisable->SetState();

	CScene* Scene = CSceneManager::GetInst()->GetScene();
	Matrix matView = Scene->GetCameraManager()->GetCurrentCamera()->GetShadowViewMatrix();
	Matrix matProj = Scene->GetCameraManager()->GetCurrentCamera()->GetShadowProjMatrix();

	m_ShadowCBuffer->SetShadowVP(matView * matProj);

	m_ShadowCBuffer->UpdateCBuffer();


	UINT Offset = 0;
	CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 0, nullptr, nullptr, &Offset);
	CDevice::GetInst()->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	CDevice::GetInst()->GetContext()->Draw(4, 0);

	m_DepthDisable->ResetState();

	m_vecGBuffer[0]->ResetTargetShader(14);
	m_vecGBuffer[2]->ResetTargetShader(16);
	m_vecLightBuffer[0]->ResetTargetShader(18);
	m_vecLightBuffer[1]->ResetTargetShader(19);
	m_vecLightBuffer[2]->ResetTargetShader(20);
	m_ShadowMapTarget->ResetTargetShader(22);

	FinalScreenTarget->ResetTarget();
}

void CRenderManager::RenderFinalScreen()
{
	CRenderTarget* FinalScreenTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("FinalScreen");

	FinalScreenTarget->SetTargetShader(21);

	m_LightBlendRenderShader->SetShader();

	m_DepthDisable->SetState();

	UINT Offset = 0;
	CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 0, nullptr, nullptr, &Offset);
	CDevice::GetInst()->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	CDevice::GetInst()->GetContext()->Draw(4, 0);

	m_DepthDisable->ResetState();

	FinalScreenTarget->ResetTargetShader(21);
}

void CRenderManager::RenderTransparent()
{
	CRenderTarget* FinalScreenTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("FinalScreen");

	FinalScreenTarget->SetTargetShader(21);
	
	auto	iter = m_RenderLayerList[(int)RenderLayerIdx::Transparent]->RenderList.begin();
	auto	iterEnd = m_RenderLayerList[(int)RenderLayerIdx::Transparent]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Render();
	}

	FinalScreenTarget->ResetTargetShader(21);
}

void CRenderManager::RenderColliderComponents()
{
	if (CEngine::GetInst()->IsColliderLayerShowEnable() == false)
		return;

	// auto iter = m_RenderLayerList[5]->RenderList.begin();
	auto iter = m_RenderLayerList[(int)RenderLayerIdx::ScreenWidgetComponent]->RenderList.begin();
	auto iterEnd = m_RenderLayerList[(int)RenderLayerIdx::ScreenWidgetComponent]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Render();
	}
}

void CRenderManager::RenderCullingLayer()
{
	if (CEngine::GetInst()->IsPickingLayerShowEnable() == false)
		return;

	auto iter = m_RenderLayerList[(int)RenderLayerIdx::DebugLayer]->RenderList.begin();
	auto iterEnd = m_RenderLayerList[(int)RenderLayerIdx::DebugLayer]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Render();
	}
}

void CRenderManager::SetBlendFactor(const std::string& Name, float r, float g,
	float b, float a)
{
	m_RenderStateManager->SetBlendFactor(Name, r, g, b, a);
}

void CRenderManager::AddBlendInfo(const std::string& Name, bool BlendEnable,
	D3D11_BLEND SrcBlend, D3D11_BLEND DestBlend, D3D11_BLEND_OP BlendOp, 
	D3D11_BLEND SrcBlendAlpha, D3D11_BLEND DestBlendAlpha, 
	D3D11_BLEND_OP BlendOpAlpha, UINT8 RenderTargetWriteMask)
{
	m_RenderStateManager->AddBlendInfo(Name, BlendEnable, SrcBlend, DestBlend,
		BlendOp, SrcBlendAlpha, DestBlendAlpha, BlendOpAlpha, RenderTargetWriteMask);
}

bool CRenderManager::CreateBlendState(const std::string& Name, 
	bool AlphaToCoverageEnable, bool IndependentBlendEnable)
{
	return m_RenderStateManager->CreateBlendState(Name, AlphaToCoverageEnable, IndependentBlendEnable);
}

CRenderState* CRenderManager::FindRenderState(const std::string& Name)
{
	return m_RenderStateManager->FindRenderState(Name);
}

void CRenderManager::RenderDefaultInstancingInfo()
{
	for (int i = 0; i < m_RenderLayerList[1]->InstancingIndex; ++i)
	{
		// Material Slot 수만큼 반복한다.
		int	SlotCount = 0;
		if (m_RenderLayerList[1]->m_vecInstancing[i]->Mesh->GetMeshType() == Mesh_Type::Static)
		{
			SlotCount = ((CStaticMeshComponent*)m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.back())->GetMaterialSlotCount();
		}

		else if (m_RenderLayerList[1]->m_vecInstancing[i]->Mesh->GetMeshType() == Mesh_Type::Animation)
		{
			SlotCount = ((CAnimationMeshComponent*)m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.back())->GetMaterialSlotCount();
		}

		for (int j = 0; j < SlotCount; ++j)
		{
			auto	iter = m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.begin();
			auto	iterEnd = m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.end();

			std::vector<Instancing3DInfo>	vecInfo;
			vecInfo.reserve(m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.size());
			
			// ShadowBuffer
			std::vector<Instancing3DInfo>	vecShadowInfo;
			vecShadowInfo.reserve(m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.size());

			CMaterial* Material = nullptr;

			for (; iter != iterEnd; ++iter)
			{
				if (m_RenderLayerList[1]->m_vecInstancing[i]->Mesh->GetMeshType() == Mesh_Type::Static)
				{
					Material = ((CStaticMeshComponent*)(*iter))->GetMaterial(j);
				}

				else if (m_RenderLayerList[1]->m_vecInstancing[i]->Mesh->GetMeshType() == Mesh_Type::Animation)
				{
					Material = ((CAnimationMeshComponent*)(*iter))->GetMaterial(j);
				}

				Instancing3DInfo	Info = {};

				(*iter)->SetInstancingInfo(&Info);
				Material->GetCBuffer()->SetInstancingInfo(&Info);

				// 페이퍼번 정보
				if (Info.MtrlPaperBurnEnable == 1)
				{
					CPaperBurnComponent* PaperBurn = (*iter)->GetGameObject()->FindComponentFromType<CPaperBurnComponent>();
					if (PaperBurn)
					{
						PaperBurn->SetInstancingInfo(&Info);
					}
				}

				vecInfo.push_back(Info);

				Instancing3DInfo	ShadowInfo = {};

				(*iter)->SetInstancingShadowInfo(&ShadowInfo);
				Material->GetCBuffer()->SetInstancingInfo(&ShadowInfo);

				vecShadowInfo.push_back(ShadowInfo);
			}

			m_RenderLayerList[1]->m_vecInstancing[i]->Buffer->UpdateBuffer(&vecInfo[0],
				(int)vecInfo.size());
			m_RenderLayerList[1]->m_vecInstancing[i]->ShadowBuffer->UpdateBuffer(&vecShadowInfo[0],
				(int)vecShadowInfo.size());
		}
	}
}

void CRenderManager::RenderDefaultInstancing()
{
	for (int i = 0; i < m_RenderLayerList[1]->InstancingIndex; ++i)
	{
		// Material Slot 수만큼 반복한다.
		int	SlotCount = 0;
		if (m_RenderLayerList[1]->m_vecInstancing[i]->Mesh->GetMeshType() == Mesh_Type::Static)
		{
			SlotCount = ((CStaticMeshComponent*)m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.back())->GetMaterialSlotCount();
		}

		else if (m_RenderLayerList[1]->m_vecInstancing[i]->Mesh->GetMeshType() == Mesh_Type::Animation)
		{
			SlotCount = ((CAnimationMeshComponent*)m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.back())->GetMaterialSlotCount();
		}

		for (int j = 0; j < SlotCount; ++j)
		{
			CMaterial* Material = nullptr;

			if (m_RenderLayerList[1]->m_vecInstancing[i]->Mesh->GetMeshType() == Mesh_Type::Static)
			{
				Material = ((CStaticMeshComponent*)m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.back())->GetMaterial(j);
			}

			else if (m_RenderLayerList[1]->m_vecInstancing[i]->Mesh->GetMeshType() == Mesh_Type::Animation)
			{
				Material = ((CAnimationMeshComponent*)m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.back())->GetMaterial(j);
			}

			if (Material)
				Material->RenderTexture();

			if (m_RenderLayerList[1]->m_vecInstancing[i]->Animation)
			{
				((CAnimationMesh*)m_RenderLayerList[1]->m_vecInstancing[i]->Mesh)->SetBoneShader();
			}

			m_Standard3DInstancingShader->SetShader();

			m_RenderLayerList[1]->m_vecInstancing[i]->CBuffer->UpdateCBuffer();

			m_RenderLayerList[1]->m_vecInstancing[i]->Buffer->SetShader();

			m_RenderLayerList[1]->m_vecInstancing[i]->Mesh->RenderInstancing((int)m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.size(), j);


			m_RenderLayerList[1]->m_vecInstancing[i]->Buffer->ResetShader();



			if (m_RenderLayerList[1]->m_vecInstancing[i]->Animation)
			{
				((CAnimationMesh*)m_RenderLayerList[1]->m_vecInstancing[i]->Mesh)->ResetBoneShader();
			}


			if (Material)
				Material->Reset();

		}
	}
}

void CRenderManager::RenderDefaultInstancingShadow()
{
	for (int i = 0; i < m_RenderLayerList[1]->InstancingIndex; ++i)
	{
		// Material Slot 수만큼 반복한다.
		int	SlotCount = 0;
		if (m_RenderLayerList[1]->m_vecInstancing[i]->Mesh->GetMeshType() == Mesh_Type::Static)
		{
			SlotCount = ((CStaticMeshComponent*)m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.back())->GetMaterialSlotCount();
		}

		else if (m_RenderLayerList[1]->m_vecInstancing[i]->Mesh->GetMeshType() == Mesh_Type::Animation)
		{
			SlotCount = ((CAnimationMeshComponent*)m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.back())->GetMaterialSlotCount();
		}

		for (int j = 0; j < SlotCount; ++j)
		{
			if (m_RenderLayerList[1]->m_vecInstancing[i]->Animation)
			{
				((CAnimationMesh*)m_RenderLayerList[1]->m_vecInstancing[i]->Mesh)->SetBoneShader();
			}

			m_ShadowMapInstancingShader->SetShader();

			m_RenderLayerList[1]->m_vecInstancing[i]->CBuffer->UpdateCBuffer();

			m_RenderLayerList[1]->m_vecInstancing[i]->ShadowBuffer->SetShader();

			m_RenderLayerList[1]->m_vecInstancing[i]->Mesh->RenderInstancing((int)m_RenderLayerList[1]->m_vecInstancing[i]->RenderList.size(), j);


			m_RenderLayerList[1]->m_vecInstancing[i]->ShadowBuffer->ResetShader();



			if (m_RenderLayerList[1]->m_vecInstancing[i]->Animation)
			{
				((CAnimationMesh*)m_RenderLayerList[1]->m_vecInstancing[i]->Mesh)->ResetBoneShader();
			}
		}
	}
}

bool CRenderManager::Sortlayer(RenderLayer* Src, RenderLayer* Dest)
{
	return Src->LayerPriority < Dest->LayerPriority;
}
