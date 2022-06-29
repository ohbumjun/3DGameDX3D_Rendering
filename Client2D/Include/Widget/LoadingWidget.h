#pragma once

#include "Widget/WidgetWindow.h"
#include "Widget/ProgressBar.h"
#include "Widget/Image.h"

class CLoadingWidget :
    public CWidgetWindow
{
	friend class CViewport;

protected:
	CLoadingWidget();
	CLoadingWidget(const CLoadingWidget& window);
	virtual ~CLoadingWidget();

private:
	CSharedPtr<CImage>	m_Back;
	CSharedPtr<CProgressBar>	m_LoadingBar;

public:
	void SetLoadingPercent(float Percent)
	{
		m_LoadingBar->SetPercent(Percent);
	}

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void Render();
	virtual CLoadingWidget* Clone();
};

