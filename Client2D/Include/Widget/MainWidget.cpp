#include "MainWidget.h"
#include "Engine.h"

CMainWidget::CMainWidget()
{
}

CMainWidget::CMainWidget(const CMainWidget& window) :
    CWidgetWindow(window)
{
    m_Button = FindWidget<CButton>("Button");
    m_Button1 = FindWidget<CButton>("Button1");
    m_Button1Text = FindWidget<CText>("Text");
    m_Hour = FindWidget<CNumber>("Hour");
    m_Minute = FindWidget<CNumber>("Minute");
    m_Second = FindWidget<CNumber>("Second");
    m_FPSText = FindWidget<CText>("FPSText");
}

CMainWidget::~CMainWidget()
{
}

void CMainWidget::Start()
{
}

bool CMainWidget::Init()
{
    if (!CWidgetWindow::Init())
        return false;

    SetSize(1280.f, 720.f);

    m_Button = CreateWidget<CButton>("Button");

    m_Button->SetPos(400.f, 300.f);
    m_Button->SetSize(200.f, 50.f);
    m_Button->SetTexture(Button_State::Normal, "StartButton", TEXT("Start.png"));
    m_Button->SetTexture(Button_State::MouseOn, "StartButton", TEXT("Start.png"));
    m_Button->SetTexture(Button_State::Click, "StartButton", TEXT("Start.png"));

    m_Button->SetTextureTint(Button_State::Normal, 220, 220, 220, 255);
    m_Button->SetTextureTint(Button_State::MouseOn, 255, 255, 255, 255);
    m_Button->SetTextureTint(Button_State::Click, 150, 150, 150, 255);

    m_Button->SetSound(Button_Sound_State::MouseOn, "UI", "ButtonMouseOn", "TeemoSmile.mp3");
    m_Button->SetSound(Button_Sound_State::Click, "UI", "ButtonClick", "TeemoStartClicck.mp3");

    m_Button->SetClickCallback<CMainWidget>(this, &CMainWidget::StartButtonClick);

    m_Button1 = CreateWidget<CButton>("Button1");

    m_Button1->SetPos(50.f, 200.f);
    m_Button1->SetSize(100.f, 40.f);

    m_Button1->SetTextureTint(Button_State::Normal, 220, 220, 220, 255);
    m_Button1->SetTextureTint(Button_State::MouseOn, 255, 255, 255, 255);
    m_Button1->SetTextureTint(Button_State::Click, 150, 150, 150, 255);

    m_Button1->SetSound(Button_Sound_State::MouseOn, "UI", "ButtonMouseOn", "TeemoSmile.mp3");
    m_Button1->SetSound(Button_Sound_State::Click, "UI", "ButtonClick", "TeemoStartClicck.mp3");

    m_Button1->SetClickCallback<CMainWidget>(this, &CMainWidget::Button1Click);

    m_Button1Text = CreateWidget<CText>("Text");

    m_Button1Text->SetText(TEXT("Button1"));
    m_Button1Text->SetPos(50.f, 200.f);
    m_Button1Text->SetSize(100.f, 40.f);
    m_Button1Text->SetZOrder(1);
    m_Button1Text->SetColor(1.f, 0.f, 0.f);
    //m_Button1Text->SetOpacity(0.5f);
    m_Button1Text->SetAlignH(TEXT_ALIGN_H::Center);
    m_Button1Text->SetShadowEnable(true);
    m_Button1Text->SetShadowOffset(2.f, 2.f);

    m_Hour = CreateWidget<CNumber>("Hour");
    m_Minute = CreateWidget<CNumber>("Minute");
    m_Second = CreateWidget<CNumber>("Second");

    std::vector<TCHAR*> vecFileName;

    for (int i = 0; i < 10; ++i)
    {
        TCHAR* FileName = new TCHAR[MAX_PATH];
        memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);

        wsprintf(FileName, TEXT("Number/1.NoRed0.%d.png"), i);

        vecFileName.push_back(FileName);
    }

    m_Hour->SetTexture("Number", vecFileName);
    m_Hour->AddFrameData(10);

    for (int i = 0; i < 10; ++i)
    {
        SAFE_DELETE_ARRAY(vecFileName[i]);
    }

    vecFileName.clear();

    for (int i = 0; i < 10; ++i)
    {
        TCHAR* FileName = new TCHAR[MAX_PATH];
        memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);

        wsprintf(FileName, TEXT("Number/1.NoRed0.%d.png"), i);

        vecFileName.push_back(FileName);
    }

    m_Minute->SetTexture("Number", vecFileName);
    m_Minute->AddFrameData(10);
    m_Minute->SetPos(100.f, 0.f);

    for (int i = 0; i < 10; ++i)
    {
        SAFE_DELETE_ARRAY(vecFileName[i]);
    }

    vecFileName.clear();

    for (int i = 0; i < 10; ++i)
    {
        TCHAR* FileName = new TCHAR[MAX_PATH];
        memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);

        wsprintf(FileName, TEXT("Number/1.NoRed0.%d.png"), i);

        vecFileName.push_back(FileName);
    }

    m_Second->SetTexture("Number", vecFileName);
    m_Second->AddFrameData(10);
    m_Second->SetPos(200.f, 0.f);

    for (int i = 0; i < 10; ++i)
    {
        SAFE_DELETE_ARRAY(vecFileName[i]);
    }

    vecFileName.clear();

    m_FPSText = CreateWidget<CText>("FPSText");

    m_FPSText->SetText(TEXT("Button1"));
    m_FPSText->SetPos(900.f, 650.f);
    m_FPSText->SetSize(300.f, 40.f);
    m_FPSText->SetZOrder(1);
    m_FPSText->SetColor(1.f, 0.f, 0.f);
    //m_Button1Text->SetOpacity(0.5f);
    m_FPSText->SetAlignH(TEXT_ALIGN_H::Center);
    m_FPSText->SetShadowEnable(true);
    m_FPSText->SetShadowOffset(2.f, 2.f);

    return true;
}

void CMainWidget::Update(float DeltaTime)
{
    CWidgetWindow::Update(DeltaTime);

    SYSTEMTIME  time;

    GetLocalTime(&time);

    m_Hour->SetNumber((int)time.wHour);
    m_Minute->SetNumber((int)time.wMinute);
    m_Second->SetNumber((int)time.wSecond);

    char    FPS[256] = {};
    sprintf_s(FPS, "FPS : %.5f", CEngine::GetInst()->GetFPS());

    TCHAR   ConvertFPS[256] = {};

    int Length = MultiByteToWideChar(CP_ACP, 0, FPS, -1, 0, 0);
    MultiByteToWideChar(CP_ACP, 0, FPS, -1, ConvertFPS, Length);

    m_FPSText->SetText(ConvertFPS);
}

void CMainWidget::PostUpdate(float DeltaTime)
{
    CWidgetWindow::PostUpdate(DeltaTime);
}

void CMainWidget::Render()
{
    CWidgetWindow::Render();
}

CMainWidget* CMainWidget::Clone()
{
    return new CMainWidget(*this);
}

void CMainWidget::StartButtonClick()
{
    //MessageBox(0, TEXT("Click"), TEXT("Click"), MB_OK);
}

void CMainWidget::Button1Click()
{
}
