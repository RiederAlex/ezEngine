#pragma once

#include <EditorFramework/Assets/SimpleAssetDocument.h>
#include <GuiFoundation/PropertyGrid/PropertyMetaState.h>
#include <ToolsFoundation/Object/DocumentObjectBase.h>


class ezLUTAssetProperties : public ezReflectedClass
{
  EZ_ADD_DYNAMIC_REFLECTION(ezLUTAssetProperties, ezReflectedClass);

public:
  static void PropertyMetaStateEventHandler(ezPropertyMetaStateEvent& e);

  const char* GetInputFile() const { return m_sInput; }
  void SetInputFile(const char* szFile) { m_sInput = szFile; }

  ezString GetAbsoluteInputFilePath() const;

private:
  ezString m_sInput;
};
