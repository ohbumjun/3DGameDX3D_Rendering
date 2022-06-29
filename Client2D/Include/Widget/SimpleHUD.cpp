#include "SimpleHUD.h"

CSimpleHUD::CSimpleHUD()
{
}

CSimpleHUD::CSimpleHUD(const CSimpleHUD& window) :
    CWidgetWindow(window)
{
    m_NameText = FindWidget<CText>("NameText");
    m_HPBar = FindWidget<CProgressBar>("HPBar");
}

CSimpleHUD::~CSimpleHUD()
{
}

void CSimpleHUD::SetText(const TCHAR* Text)
{
    m_NameText->SetText(Text);
}

void CSimpleHUD::SetHPPercent(float Percent)
{
    m_HPBar->SetPercent(Percent);
}

void CSimpleHUD::SetHPDir(ProgressBar_Dir Dir)
{
    m_HPBar->SetDir(Dir);
}

void CSimpleHUD::Start()
{
}

bool CSimpleHUD::Init()
{
    if (!CWidgetWindow::Init())
        return false;

    SetSize(100.f, 50.f);

    m_NameText = CreateWidget<CText>("NameText");

    m_NameText->SetText(TEXT("Name"));
    m_NameText->SetPos(0.f, 20.f);
    m_NameText->SetSize(100.f, 30.f);
    m_NameText->SetFontSize(13.f);
    m_NameText->SetZOrder(1);
    m_NameText->SetColor(1.f, 1.f, 1.f);
    m_NameText->SetAlignH(TEXT_ALIGN_H::Center);

    m_HPBar = CreateWidget<CProgressBar>("HPBar");
    m_HPBar->SetSize(100.f, 20.f);
    m_HPBar->SetTextureTint(255, 0, 0, 255);

    return true;
}

void CSimpleHUD::Update(float DeltaTime)
{
    CWidgetWindow::Update(DeltaTime);
}

void CSimpleHUD::PostUpdate(float DeltaTime)
{
    CWidgetWindow::PostUpdate(DeltaTime);
}

void CSimpleHUD::Render()
{
    CWidgetWindow::Render();
}

CSimpleHUD* CSimpleHUD::Clone()
{
    return new CSimpleHUD(*this);
}
