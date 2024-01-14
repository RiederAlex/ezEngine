#pragma once

class EZ_EDITORENGINEPROCESSFRAMEWORK_DLL ezViewPickingMsgToEngine : public ezEditorEngineViewMsg
{
  EZ_ADD_DYNAMIC_REFLECTION(ezViewPickingMsgToEngine, ezEditorEngineViewMsg);

public:
  ezUInt16 m_uiPickPosX;
  ezUInt16 m_uiPickPosY;
};
