#pragma once
#include "Texture.h"
class CRenderTarget :
    public CTexture
{
	friend class CTextureManager;

protected:
	CRenderTarget();
	virtual ~CRenderTarget();

protected:
	ID3D11RenderTargetView* m_TargetView;
	ID3D11Texture2D* m_TargetTex;
	ID3D11DepthStencilView* m_DepthView;
	ID3D11RenderTargetView* m_PrevTargetView;
	ID3D11DepthStencilView* m_PrevDepthView;
	IDXGISurface* m_Surface;
	float	m_ClearColor[4];
	bool	m_DebugRender;
	Matrix	m_matDebugWVP;
	Vector3	m_Scale;
	Vector3	m_Pos;

public:
	IDXGISurface* GetSurface()	const
	{
		return m_Surface;
	}

	ID3D11RenderTargetView* GetTargetView()	const
	{
		return m_TargetView;
	}

	Matrix GetWVP()	const
	{
		return m_matDebugWVP;
	}

	void SetDebugRender(bool Render)
	{
		m_DebugRender = Render;
	}

	void SetPos(const Vector3& Pos)
	{
		m_Pos = Pos;
	}

	void SetScale(const Vector3& Scale)
	{
		m_Scale = Scale;
	}

public:
	bool CreateTarget(const std::string& Name, unsigned int Width,
		unsigned int Height, DXGI_FORMAT PixelFormat,
		DXGI_FORMAT DepthFormat = DXGI_FORMAT_UNKNOWN);
	void ClearTarget();
	void SetTarget(ID3D11DepthStencilView* DepthView);
	void SetTarget();
	void ResetTarget();
	void SetTargetShader();
	void ResetTargetShader();
	void SetTargetShader(int Register);
	void ResetTargetShader(int Register);
};

