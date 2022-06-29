#include "StartWidget.h"
#include "Engine.h"
#include "../Scene/LoadingScene.h"
#include "Scene/SceneManager.h"

CStartWidget::CStartWidget()
{
}

CStartWidget::CStartWidget(const CStartWidget& window) :
    CWidgetWindow(window)
{
    m_StartButton = FindWidget<CButton>("StartButton");
    m_ExitButton = FindWidget<CButton>("ExitButton");
    m_Back = FindWidget<CImage>("Back");
}

CStartWidget::~CStartWidget()
{
}

void CStartWidget::Start()
{
}

bool CStartWidget::Init()
{
    if (!CWidgetWindow::Init())
        return false;

    SetSize(1280.f, 720.f);

    m_StartButton = CreateWidget<CButton>("StartButton");
    m_ExitButton = CreateWidget<CButton>("ExitButton");
    m_Back = CreateWidget<CImage>("Back");

    m_StartButton->SetZOrder(1);
    m_ExitButton->SetZOrder(1);

    m_Back->SetTexture("StartBack", TEXT("Teemo.jpg"));
    m_Back->SetSize(1280.f, 720.f);

    m_StartButton->SetPos(1280.f / 2.f - 100.f, 720.f / 2.f + 50.f);
    m_StartButton->SetSize(200.f, 50.f);
    m_StartButton->SetTexture(Button_State::Normal, "StartButton", TEXT("Start.png"));
    m_StartButton->SetTexture(Button_State::MouseOn, "StartButton", TEXT("Start.png"));
    m_StartButton->SetTexture(Button_State::Click, "StartButton", TEXT("Start.png"));

    m_StartButton->SetTextureTint(Button_State::Normal, 220, 220, 220, 255);
    m_StartButton->SetTextureTint(Button_State::MouseOn, 255, 255, 255, 255);
    m_StartButton->SetTextureTint(Button_State::Click, 150, 150, 150, 255);

    m_StartButton->SetSound(Button_Sound_State::MouseOn, "UI", "ButtonMouseOn", "TeemoSmile.mp3");
    m_StartButton->SetSound(Button_Sound_State::Click, "UI", "ButtonClick", "TeemoStartClicck.mp3");

    m_StartButton->SetClickCallback<CStartWidget>(this, &CStartWidget::StartButtonClick);

    m_ExitButton->SetPos(1280.f / 2.f - 100.f, 720.f / 2.f - 100.f);
    m_ExitButton->SetSize(200.f, 50.f);
    m_ExitButton->SetTexture(Button_State::Normal, "EndButton", TEXT("End.png"));
    m_ExitButton->SetTexture(Button_State::MouseOn, "EndButton", TEXT("End.png"));
    m_ExitButton->SetTexture(Button_State::Click, "EndButton", TEXT("End.png"));

    m_ExitButton->SetTextureTint(Button_State::Normal, 220, 220, 220, 255);
    m_ExitButton->SetTextureTint(Button_State::MouseOn, 255, 255, 255, 255);
    m_ExitButton->SetTextureTint(Button_State::Click, 150, 150, 150, 255);

    m_ExitButton->SetSound(Button_Sound_State::MouseOn, "UI", "ButtonMouseOn", "TeemoSmile.mp3");
    m_ExitButton->SetSound(Button_Sound_State::Click, "UI", "ButtonClick", "TeemoStartClicck.mp3");

    m_ExitButton->SetClickCallback<CStartWidget>(this, &CStartWidget::ExitButtonClick);

    return true;
}

void CStartWidget::Update(float DeltaTime)
{
    CWidgetWindow::Update(DeltaTime);
}

void CStartWidget::PostUpdate(float DeltaTime)
{
    CWidgetWindow::PostUpdate(DeltaTime);
}

void CStartWidget::Render()
{
    CWidgetWindow::Render();
}

CStartWidget* CStartWidget::Clone()
{
    return new CStartWidget(*this);
}

void CStartWidget::StartButtonClick()
{
    CSceneManager::GetInst()->CreateNextScene();
    CSceneManager::GetInst()->CreateSceneMode<CLoadingScene>(false);
}

void CStartWidget::ExitButtonClick()
{
    CEngine::GetInst()->Exit();
}
