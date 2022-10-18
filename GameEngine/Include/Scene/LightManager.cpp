
#include "LightManager.h"
#include "Scene.h"
#include "../GameObject/GameObject.h"
#include "SceneResource.h"
#include "../Resource/Shader/Shader.h"
#include "../Device.h"
#include "../Component/Transform.h"
#include "../Resource/Shader/StructuredBuffer.h"
#include "../Resource/Shader/LightFowardConstantBuffer.h"

CLightManager::CLightManager()
{
}

CLightManager::~CLightManager()
{
}

void CLightManager::AddLight(CLightComponent* Light)
{
	m_LightList.push_back(Light);

	// 차후 반투명 물체들을 Forward Rendering 방식으로 그릴 때를 대비하기 위한 것
	CreateLightListStructuredBuffer();
}

void CLightManager::DeleteLight(CLightComponent* Light)
{
	auto	iter = m_LightList.begin();
	auto	iterEnd = m_LightList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == Light)
		{
			m_LightList.erase(iter);
			return;
		}
	}

	if (!m_LightList.empty())
		CreateLightListStructuredBuffer();
}

void CLightManager::DeleteLight(const std::string& Name)
{
	auto	iter = m_LightList.begin();
	auto	iterEnd = m_LightList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetName() == Name)
		{
			m_LightList.erase(iter);
			return;
		}
	}
}

void CLightManager::Start()
{
}

void CLightManager::Init()
{
	m_GlobalLight = m_Scene->CreateGameObject<CGameObject>("GlobalLight");

	m_GlobalLightComponent = m_GlobalLight->CreateComponent<CLightComponent>("Light");

	m_GlobalLightComponent->SetRelativeRotation(45.f, 90.f, 0.f);

	m_GlobalLightComponent->SetLightType(Light_Type::Dir);
	 
	//m_GlobalLightComponent->SetRelativePos(-3.f, 5.f, 0.f);

	//m_GlobalLightComponent->SetLightType(Light_Type::Point);

	//m_GlobalLightComponent->SetDistance(10.f);
	//m_GlobalLightComponent->SetAtt3(0.02f);

	m_FowardLightCBuffer = new CLightFowardConstantBuffer;
	m_FowardLightCBuffer->Init();
}

void CLightManager::Update(float DeltaTime)
{
	auto	iter = m_LightList.begin();
	auto	iterEnd = m_LightList.end();

	for (; iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			iter = m_LightList.erase(iter);
			iterEnd = m_LightList.end();

			continue;
		}

		++iter;
	}
}

void CLightManager::PostUpdate(float DeltaTime)
{
}

void CLightManager::SetShader()
{
	auto	iter = m_LightList.begin();
	auto	iterEnd = m_LightList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (!(*iter)->IsEnable())
			continue;

		(*iter)->SetShader();
	}
}

void CLightManager::Destroy()
{
	std::list<CSharedPtr<CLightComponent>>	List = m_LightList;
	m_LightList.clear();
	List.clear();
	m_GlobalLightComponent = nullptr;
	m_GlobalLight = nullptr;
}

void CLightManager::Render()
{
	CShader* Shader = m_Scene->GetResource()->FindShader("LightAccShader");

	Shader->SetShader();

	auto	iter = m_LightList.begin();
	auto	iterEnd = m_LightList.end();

	bool	SendTransform = false;

	for (; iter != iterEnd; ++iter)
	{
		if (!(*iter)->IsEnable())
			continue;

		if (!SendTransform)
		{
			(*iter)->GetTransform()->SetTransform();
			SendTransform = true;
		}

		(*iter)->SetShader();

		UINT Offset = 0;
		CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 0, nullptr, nullptr, &Offset);
		CDevice::GetInst()->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

		CDevice::GetInst()->GetContext()->Draw(4, 0);
	}
}

void CLightManager::SetForwardRenderShader()
{
	// Forward 렌더링을 적용하기 위한 구조화 버퍼 정보를 채워준다
	size_t LightSize = m_LightList.size();

	// 상수 버퍼 정보를 넘긴다.
	m_FowardLightCBuffer->SetLightCount((int)LightSize);
	m_FowardLightCBuffer->UpdateCBuffer();

	// 전역 조명 상수버퍼 바인드
	// m_GlobalLightCBuffer->UpdateCBuffer();

	m_LightListBufferData.clear();
	m_LightListBufferData.resize(LightSize);

	int Count = 0;

	auto iter      = m_LightList.begin();
	auto iterEnd = m_LightList.end();

	for (; iter != iterEnd; ++iter)
	{
		m_LightListBufferData[Count].Pos = (*iter)->GetLightPos();
		m_LightListBufferData[Count].Color = (*iter)->GetLightColor();
		m_LightListBufferData[Count].LightType = (int)(*iter)->GetLightType();
		m_LightListBufferData[Count].Dir = (*iter)->GetLightDir();
		m_LightListBufferData[Count].Distance = (*iter)->GetLightDistance();
		m_LightListBufferData[Count].AngleIn = (*iter)->GetLightAngleIn();
		m_LightListBufferData[Count].AngleOut = (*iter)->GetLightAngleOut();
		m_LightListBufferData[Count].Att1 = (*iter)->GetLightAtt1();
		m_LightListBufferData[Count].Att2 = (*iter)->GetLightAtt2();
		m_LightListBufferData[Count].Att3 = (*iter)->GetLightAtt3();

		++Count;
	}

	m_ForwardLightListStructuredBuffer->UpdateBuffer(&m_LightListBufferData[0], (int)m_LightList.size());
	m_ForwardLightListStructuredBuffer->SetShader();
}

void CLightManager::ResetForwardRenderShader()
{
	m_ForwardLightListStructuredBuffer->ResetShader();
}

void CLightManager::CreateLightListStructuredBuffer()
{
	SAFE_DELETE(m_ForwardLightListStructuredBuffer);

	// 13번으로 세팅
	m_ForwardLightListStructuredBuffer = new CStructuredBuffer;
	m_ForwardLightListStructuredBuffer->Init("LightListStructuredBuffer", sizeof(LightCBuffer), (unsigned int)m_LightList.size(),
		13, true, (int)Buffer_Shader_Type::Pixel);
}
