#pragma once

#include "Widget/WidgetWindow.h"
#include "Widget/Button.h"
#include "Widget/Image.h"

class CStartWidget :
    public CWidgetWindow
{
	friend class CViewport;

protected:
	CStartWidget();
	CStartWidget(const CStartWidget& window);
	virtual ~CStartWidget();

private:
	CSharedPtr<CButton>	m_StartButton;
	CSharedPtr<CButton>	m_ExitButton;
	CSharedPtr<CImage>	m_Back;

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void Render();
	virtual CStartWidget* Clone();

public:
	void StartButtonClick();
	void ExitButtonClick();
};

