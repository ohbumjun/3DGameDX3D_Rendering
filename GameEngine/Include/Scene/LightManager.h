#pragma once

#include "../GameInfo.h"
#include "../Component/LightComponent.h"

class CLightManager
{
	friend class CScene;

private:
	CLightManager();
	~CLightManager();

private:
	class CScene* m_Scene;
	CSharedPtr<class CGameObject>	m_GlobalLight;
	CSharedPtr<CLightComponent>	m_GlobalLightComponent;

private:
	std::list<CSharedPtr<CLightComponent>>	m_LightList;

	// 반투명 물체들에 Forward Rendering을 적용하기 위한 것
private :
	std::vector<LightCBuffer> m_LightListBufferData;

public:
	class CGameObject* GetGlobalLightObj()	const
	{
		return m_GlobalLight;
	}

	class CLightComponent* GetGlobalLightComponent()	const
	{
		return m_GlobalLightComponent;
	}

public:
	void AddLight(CLightComponent* Light);
	void DeleteLight(CLightComponent* Light);
	void DeleteLight(const std::string& Name);

public:
	void Start();
	void Init();
	void Update(float DeltaTime);
	void PostUpdate(float DeltaTime);
	void SetShader();
	void Destroy();
	void Render();

	void SetForwardRenderShader();
};

