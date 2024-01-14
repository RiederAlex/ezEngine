#include <SharedPluginRmlUi/Common/Messages.h>

// clang-format off
EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezEditorEngineForceRefreshMsg, 1, ezRTTIDefaultAllocator<ezEditorEngineForceRefreshMsg>)
EZ_END_DYNAMIC_REFLECTED_TYPE;

EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezEditorEngineAutoFillDataBindingMsg, 1, ezRTTIDefaultAllocator<ezEditorEngineAutoFillDataBindingMsg>)
{
  EZ_BEGIN_PROPERTIES
  {
    EZ_MAP_MEMBER_PROPERTY("Bindings", m_Bindings),
  }
  EZ_END_PROPERTIES;
}
EZ_END_DYNAMIC_REFLECTED_TYPE;

EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezEditorEngineApplyDataBindingMsg, 1, ezRTTIDefaultAllocator<ezEditorEngineApplyDataBindingMsg>)
EZ_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on
