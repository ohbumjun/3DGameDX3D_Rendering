#pragma once

#include "IMGUIWindow.h"

class CDetailWindow :
	public CIMGUIWindow
{
public:
	CDetailWindow();
	virtual ~CDetailWindow();

private:
	class CIMGUITextInput* m_PosX;
	class CIMGUITextInput* m_PosY;
	class CIMGUITextInput* m_PosZ;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

private:
	void PositionXCallback();
	void PositionYCallback();
	void PositionZCallback();
};

