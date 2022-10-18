#pragma once

#include "SceneComponent.h"
#include "../Resource/Shader/LightConstantBuffer.h"

class CLightComponent :
    public CSceneComponent
{
	friend class CGameObject;
	friend class CCameraManager;

protected:
	CLightComponent();
	CLightComponent(const CLightComponent& com);
	virtual ~CLightComponent();

protected:
	CLightConstantBuffer* m_CBuffer;

	// Getter
public:
	Light_Type GetLightType()	const
	{
		return m_CBuffer->GetLightType();
	}
	const Vector3& GetLightPos() const
	{
		return m_CBuffer->GetLightPos();
	}

	const Vector3& GetLightDir() const
	{
		return m_CBuffer->GetLightDir();
	}

	const Vector4& GetLightColor() const
	{
		return m_CBuffer->GetLightColor();
	}

	float GetLightDistance()	const
	{
		return m_CBuffer->GetLightDistance();
	}

	float GetLightAngleIn() const
	{
		return m_CBuffer->GetLightAngleIn();
	}

	float GetLightAngleOut() const
	{
		return m_CBuffer->GetLightAngleOut();
	}

	float GetLightAtt1() const
	{
		return m_CBuffer->GetLightAtt1();
	}

	float GetLightAtt2() const
	{
		return m_CBuffer->GetLightAtt2();
	}

	float GetLightAtt3() const
	{
		return m_CBuffer->GetLightAtt3();
	}
	// Setter
public :
	void SetColor(const Vector4& Color)
	{
		m_CBuffer->SetColor(Color);
	}

	void SetLightType(Light_Type LightType)
	{
		m_CBuffer->SetLightType(LightType);
	}

	void SetPos(const Vector3& Pos)
	{
		m_CBuffer->SetPos(Pos);
	}

	void SetDir(const Vector3& Dir)
	{
		m_CBuffer->SetDir(Dir);
	}

	void SetDistance(float Distance)
	{
		m_CBuffer->SetDistance(Distance);
	}

	void SetAngleIn(float AngleIn)
	{
		m_CBuffer->SetAngleIn(AngleIn);
	}

	void SetAngleOut(float AngleOut)
	{
		m_CBuffer->SetAngleOut(AngleOut);
	}

	void SetAtt1(float Att1)
	{
		m_CBuffer->SetAtt1(Att1);
	}

	void SetAtt2(float Att2)
	{
		m_CBuffer->SetAtt2(Att2);
	}

	void SetAtt3(float Att3)
	{
		m_CBuffer->SetAtt3(Att3);
	}

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CLightComponent* Clone();
	virtual void Save(FILE* File);
	virtual void Load(FILE* File);

public:
	void SetShader();
};

