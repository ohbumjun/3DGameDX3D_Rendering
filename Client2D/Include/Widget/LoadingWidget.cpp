#include "LoadingWidget.h"
#include "Engine.h"
#include "../Scene/MainScene.h"
#include "Scene/SceneManager.h"

CLoadingWidget::CLoadingWidget()
{
}

CLoadingWidget::CLoadingWidget(const CLoadingWidget& window) :
    CWidgetWindow(window)
{
    m_Back = FindWidget<CImage>("Back");
    m_LoadingBar = FindWidget<CProgressBar>("LoadingBar");
}

CLoadingWidget::~CLoadingWidget()
{
}

void CLoadingWidget::Start()
{
}

bool CLoadingWidget::Init()
{
    if (!CWidgetWindow::Init())
        return false;

    SetSize(1280.f, 720.f);

    m_Back = CreateWidget<CImage>("Back");
    m_LoadingBar = CreateWidget<CProgressBar>("LoadingBar");

    m_Back->SetTexture("LoadingBack", TEXT("LoadingBack.jpg"));
    m_Back->SetSize(1280.f, 720.f);

    m_LoadingBar->SetPos(150.f, 100.f);
    m_LoadingBar->SetSize(980.f, 40.f);
    m_LoadingBar->SetTexture("LoadingBar", TEXT("HPBar.png"));
    m_LoadingBar->SetZOrder(1);
    m_LoadingBar->SetPercent(0.f);

    return true;
}

void CLoadingWidget::Update(float DeltaTime)
{
    CWidgetWindow::Update(DeltaTime);
}

void CLoadingWidget::PostUpdate(float DeltaTime)
{
    CWidgetWindow::PostUpdate(DeltaTime);
}

void CLoadingWidget::Render()
{
    CWidgetWindow::Render();
}

CLoadingWidget* CLoadingWidget::Clone()
{
    return new CLoadingWidget(*this);
}
