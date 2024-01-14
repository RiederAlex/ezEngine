#pragma once

#include <EditorEngineProcessFramework/EngineProcess/EngineProcessMessages.h>
#include <SharedPluginRmlUi/SharedPluginRmlUiDLL.h>

class EZ_SHAREDPLUGINRMLUI_DLL ezEditorEngineForceRefreshMsg : public ezEditorEngineDocumentMsg
{
  EZ_ADD_DYNAMIC_REFLECTION(ezEditorEngineForceRefreshMsg, ezEditorEngineDocumentMsg);
};

class EZ_SHAREDPLUGINRMLUI_DLL ezEditorEngineAutoFillDataBindingMsg : public ezEditorEngineDocumentMsg
{
  EZ_ADD_DYNAMIC_REFLECTION(ezEditorEngineAutoFillDataBindingMsg, ezEditorEngineDocumentMsg);

public:
  ezVariantDictionary m_Bindings;
};

class EZ_SHAREDPLUGINRMLUI_DLL ezEditorEngineApplyDataBindingMsg : public ezEditorEngineAutoFillDataBindingMsg
{
  EZ_ADD_DYNAMIC_REFLECTION(ezEditorEngineApplyDataBindingMsg, ezEditorEngineAutoFillDataBindingMsg);
};
