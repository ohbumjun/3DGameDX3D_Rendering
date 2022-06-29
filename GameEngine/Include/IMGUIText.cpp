#include "IMGUIText.h"

CIMGUIText::CIMGUIText()    :
    m_Text{},
    m_wText{},
    m_TextUTF8{}
{
}

CIMGUIText::~CIMGUIText()
{
}

bool CIMGUIText::Init()
{
    return true;
}

void CIMGUIText::Render()
{
    ImGui::TextColored(m_Color.Value, m_TextUTF8);
}
