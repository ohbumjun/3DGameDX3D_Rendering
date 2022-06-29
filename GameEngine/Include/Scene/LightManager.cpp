
#include "LightManager.h"
#include "Scene.h"
#include "../GameObject/GameObject.h"
#include "SceneResource.h"
#include "../Resource/Shader/Shader.h"
#include "../Device.h"
#include "../Component/Transform.h"

CLightManager::CLightManager()
{
}

CLightManager::~CLightManager()
{
}

void CLightManager::AddLight(CLightComponent* Light)
{
	m_LightList.push_back(Light);
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
