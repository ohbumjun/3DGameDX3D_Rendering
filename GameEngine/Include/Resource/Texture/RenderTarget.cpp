
#include "RenderTarget.h"
#include "../../Device.h"

CRenderTarget::CRenderTarget()	:
	m_TargetView(nullptr),
	m_TargetTex(nullptr),
	m_PrevTargetView(nullptr),
	m_PrevDepthView(nullptr),
	m_Surface(nullptr),
	m_ClearColor{},
	m_DebugRender(false),
	m_DepthView(nullptr)
{
	m_ImageType = Image_Type::RenderTarget;
}

CRenderTarget::~CRenderTarget()
{
	SAFE_RELEASE(m_DepthView);
	SAFE_RELEASE(m_Surface);
	SAFE_RELEASE(m_PrevDepthView);
	SAFE_RELEASE(m_PrevTargetView);
	SAFE_RELEASE(m_TargetTex);
	SAFE_RELEASE(m_TargetView);
}

bool CRenderTarget::CreateTarget(const std::string& Name, 
	unsigned int Width, unsigned int Height, DXGI_FORMAT PixelFormat,
	DXGI_FORMAT DepthFormat)
{
	SetName(Name);

	// Target용 Texture 생성
	D3D11_TEXTURE2D_DESC	Desc = {};

	Desc.Width = Width;
	Desc.Height = Height;
	Desc.ArraySize = 1;
	Desc.MipLevels = 1;
	Desc.SampleDesc.Count = 4;
	Desc.SampleDesc.Quality = 0;
	Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	Desc.Format = PixelFormat;
	Desc.Usage = D3D11_USAGE_DEFAULT;

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateTexture2D(&Desc, nullptr, &m_TargetTex)))
		return false;

	TextureResourceInfo* Info = new TextureResourceInfo;

	Info->Width = Width;
	Info->Height = Height;
	m_vecTextureInfo.push_back(Info);

	m_TargetTex->QueryInterface(__uuidof(IDXGISurface), (void**)&m_Surface);

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateShaderResourceView(
		m_TargetTex, nullptr, &Info->SRV)))
		return false;

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateRenderTargetView(m_TargetTex,
		nullptr, &m_TargetView)))
		return false;

	if (DepthFormat != DXGI_FORMAT_UNKNOWN)
	{
		// 깊이버퍼를 만든다.
		D3D11_TEXTURE2D_DESC	DepthDesc = {};

		DepthDesc.Width = Width;
		DepthDesc.Height = Height;
		DepthDesc.ArraySize = 1;

		DepthDesc.Format = DepthFormat;
		DepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		DepthDesc.Usage = D3D11_USAGE_DEFAULT;
		DepthDesc.SampleDesc.Count = 4;
		DepthDesc.SampleDesc.Quality = 0;
		DepthDesc.MipLevels = 1;

		ID3D11Texture2D* DepthBuffer = nullptr;

		if (FAILED(CDevice::GetInst()->GetDevice()->CreateTexture2D(&DepthDesc, nullptr, &DepthBuffer)))
			return false;

		if (FAILED(CDevice::GetInst()->GetDevice()->CreateDepthStencilView(DepthBuffer, nullptr, &m_DepthView)))
			return false;

		SAFE_RELEASE(DepthBuffer);
	}

	return true;
}

void CRenderTarget::ClearTarget()
{
	CDevice::GetInst()->GetContext()->ClearRenderTargetView(m_TargetView, m_ClearColor);

	if (m_DepthView)
		CDevice::GetInst()->GetContext()->ClearDepthStencilView(m_DepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void CRenderTarget::SetTarget(ID3D11DepthStencilView* DepthView)
{
	CDevice::GetInst()->GetContext()->OMGetRenderTargets(1, &m_PrevTargetView, &m_PrevDepthView);

	ID3D11DepthStencilView* Depth = m_PrevDepthView;

	if (DepthView)
		Depth = DepthView;

	CDevice::GetInst()->GetContext()->OMSetRenderTargets(1, &m_TargetView, Depth);
}

void CRenderTarget::SetTarget()
{
	CDevice::GetInst()->GetContext()->OMGetRenderTargets(1, &m_PrevTargetView, &m_PrevDepthView);

	ID3D11DepthStencilView* Depth = m_PrevDepthView;

	if (m_DepthView)
		Depth = m_DepthView;

	CDevice::GetInst()->GetContext()->OMSetRenderTargets(1, &m_TargetView, Depth);
}

void CRenderTarget::ResetTarget()
{
	CDevice::GetInst()->GetContext()->OMSetRenderTargets(1, &m_PrevTargetView, m_PrevDepthView);
	SAFE_RELEASE(m_PrevTargetView);
	SAFE_RELEASE(m_PrevDepthView);
}

void CRenderTarget::SetTargetShader()
{
	CDevice::GetInst()->GetContext()->PSSetShaderResources(10, 1, &m_vecTextureInfo[0]->SRV);
}

void CRenderTarget::ResetTargetShader()
{
	ID3D11ShaderResourceView* SRV = nullptr;
	CDevice::GetInst()->GetContext()->PSSetShaderResources(10, 1, &SRV);
}

void CRenderTarget::SetTargetShader(int Register)
{
	CDevice::GetInst()->GetContext()->PSSetShaderResources(Register, 1, &m_vecTextureInfo[0]->SRV);
}

void CRenderTarget::ResetTargetShader(int Register)
{
	ID3D11ShaderResourceView* SRV = nullptr;
	CDevice::GetInst()->GetContext()->PSSetShaderResources(Register, 1, &SRV);
}
