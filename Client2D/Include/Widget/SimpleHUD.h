#pragma once

#include "Widget/WidgetWindow.h"
#include "Widget/Button.h"
#include "Widget/Text.h"
#include "Widget/ProgressBar.h"

class CSimpleHUD :
    public CWidgetWindow
{
	friend class CViewport;
	friend class CWidgetComponent;

protected:
	CSimpleHUD();
	CSimpleHUD(const CSimpleHUD& window);
	virtual ~CSimpleHUD();

private:
	CSharedPtr<CButton>	m_Button;
	CSharedPtr<CText>	m_NameText;
	CSharedPtr<CProgressBar>	m_HPBar;

public:
	void SetText(const TCHAR* Text);
	void SetHPPercent(float Percent);
	void SetHPDir(ProgressBar_Dir Dir);

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void Render();
	virtual CSimpleHUD* Clone();
};

