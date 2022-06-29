#include "MouseAttack.h"

CMouseAttack::CMouseAttack()
{
}

CMouseAttack::CMouseAttack(const CMouseAttack& window) :
    CWidgetWindow(window)
{
    m_Image = FindWidget<CImage>("Image");
}

CMouseAttack::~CMouseAttack()
{
}

void CMouseAttack::Start()
{
}

bool CMouseAttack::Init()
{
    if (!CWidgetWindow::Init())
        return false;

    SetSize(32.f, 31.f);

    m_Image = CreateWidget<CImage>("Image");

    std::vector<TCHAR*> vecFileName;

    for (int i = 0; i <= 6; ++i)
    {
        TCHAR* FileName = new TCHAR[MAX_PATH];
        memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);

        wsprintf(FileName, TEXT("Mouse/Temp/Attack/000%d.png"), i);

        vecFileName.push_back(FileName);
    }

    m_Image->SetTexture("MouseAttack", vecFileName);
    m_Image->SetSize(30.f, 30.f);

    m_Image->AddFrameData(7);

    for (int i = 0; i <= 6; ++i)
    {
        SAFE_DELETE_ARRAY(vecFileName[i]);
    }

    return true;
}

void CMouseAttack::Update(float DeltaTime)
{
    CWidgetWindow::Update(DeltaTime);
}

void CMouseAttack::PostUpdate(float DeltaTime)
{
    CWidgetWindow::PostUpdate(DeltaTime);
}

void CMouseAttack::Render()
{
    CWidgetWindow::Render();
}

CMouseAttack* CMouseAttack::Clone()
{
    return new CMouseAttack(*this);
}
