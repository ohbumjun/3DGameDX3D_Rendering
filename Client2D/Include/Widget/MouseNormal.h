#pragma once

#include "Widget/WidgetWindow.h"
#include "Widget/Image.h"

class CMouseNormal :
    public CWidgetWindow
{
	friend class CViewport;
	friend class CWidgetComponent;
	friend class CEngine;

protected:
	CMouseNormal();
	CMouseNormal(const CMouseNormal& window);
	virtual ~CMouseNormal();

private:
	CSharedPtr<CImage>	m_Image;

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void Render();
	virtual CMouseNormal* Clone();
};

