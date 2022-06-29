#pragma once

#include "ConstantBufferBase.h"

class CMaterialConstantBuffer :
    public CConstantBufferBase
{
public:
	CMaterialConstantBuffer();
	CMaterialConstantBuffer(const CMaterialConstantBuffer& Buffer);
	virtual ~CMaterialConstantBuffer();

protected:
	MaterialCBuffer	m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CMaterialConstantBuffer* Clone();

public:
	void SetInstancingInfo(Instancing3DInfo* Info)
	{
		Info->MtrlBaseColor = m_BufferData.BaseColor;
		Info->MtrlAmbientColor = m_BufferData.AmbientColor;
		Info->MtrlSpecularColor = m_BufferData.SpecularColor;
		Info->MtrlEmissiveColor = m_BufferData.EmissiveColor;
		Info->MtrlOpacity = m_BufferData.Opacity;
		Info->MtrlPaperBurnEnable = m_BufferData.PaperBurnEnable;
		Info->MtrlBumpEnable = m_BufferData.BumpEnable;
		Info->MtrlAnimation3DEnable = m_BufferData.Animation3DEnable;
		Info->MtrlSpecularTex = m_BufferData.SpecularTex;
		Info->MtrlEmissiveTex = m_BufferData.EmissiveTex;
		Info->MtrlReceiveDecal = m_BufferData.ReceiveDecal;
	}

	void SetBaseColor(const Vector4& BaseColor)
	{
		m_BufferData.BaseColor = BaseColor;
	}

	void SetAmbientColor(const Vector4& Color)
	{
		m_BufferData.AmbientColor = Color;
	}

	void SetSpecularColor(const Vector4& Color)
	{
		m_BufferData.SpecularColor = Color;
	}

	void SetEmissiveColor(const Vector4& Color)
	{
		m_BufferData.EmissiveColor = Color;
	}

	void SetOpacity(float Opacity)
	{
		m_BufferData.Opacity = Opacity;
	}

	void SetPaperBurn(bool Enable)
	{
		m_BufferData.PaperBurnEnable = Enable ? 1 : 0;
	}

	void SetBump(bool Enable)
	{
		m_BufferData.BumpEnable = Enable ? 1 : 0;
	}

	void SetAnimation3D(bool Enable)
	{
		m_BufferData.Animation3DEnable = Enable ? 1 : 0;
	}

	void SetSpecularTex(bool Enable)
	{
		m_BufferData.SpecularTex = Enable ? 1 : 0;
	}

	void SetEmissiveTex(bool Enable)
	{
		m_BufferData.EmissiveTex = Enable ? 1 : 0;
	}

	void SetReceiveDecal(bool Enable)
	{
		m_BufferData.ReceiveDecal = Enable ? 1 : 0;
	}
};

