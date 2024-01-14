#include "RmlUiPlugin/RmlUiContext.h"


#include "RmlUiPlugin/RmlUiDataBinding.h"


#include <EnginePluginRmlUi/EnginePluginRmlUiPCH.h>

#include <EnginePluginRmlUi/RmlUiAsset/RmlUiContext.h>
#include <EnginePluginRmlUi/RmlUiAsset/RmlUiView.h>
#include <RmlUi/Core/DataModelHandle.h>
#include <RmlUiPlugin/Resources/RmlUiResource.h>

// clang-format off
EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezRmlUiDocumentContext, 1, ezRTTIDefaultAllocator<ezRmlUiDocumentContext>)
{
  EZ_BEGIN_PROPERTIES
  {
    EZ_CONSTANT_PROPERTY("DocumentType", (const char*) "RmlUi"),
  }
  EZ_END_PROPERTIES;
}
EZ_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

ezRmlUiDocumentContext::ezRmlUiDocumentContext()
  : ezEngineProcessDocumentContext(ezEngineProcessDocumentContextFlags::CreateWorld)
{
}

ezRmlUiDocumentContext::~ezRmlUiDocumentContext() = default;

class MockDataBinding : public ezRmlUiDataBinding
{
private:
  Rml::DataModelHandle m_hDataModel;

public:
  Rml::Vector<ezVariant> MockData;
  ezVariant TestData;

  explicit MockDataBinding()
  {
  }

  ezResult Initialize(Rml::Context& ref_context) override
  {
    Rml::DataModelConstructor constructor = ref_context.CreateDataModel("PlayerStats");
    if (!constructor)
      return EZ_FAILURE;

    constructor.RegisterScalar<ezVariant>(
      [](const ezVariant& value, Rml::Variant& variant)
      { variant = value.ConvertTo<ezString>(); });

    TestData = 10;

    constructor.Bind("HP", &TestData);
    //constructor.Bind("MAXHP", &TestData);
    m_hDataModel = constructor.GetModelHandle();
    return EZ_SUCCESS;
  }

  void Deinitialize(Rml::Context& ref_context) override
  {
  }

  void Update()
  {
  }
};

void CollectDataModel(Rml::Element* pElement,ezVariantDictionary& dict)
{
  if(!pElement) return;

  Rml::Variant* pDataModelAttribute = pElement->GetAttribute("data-model");
  if(pDataModelAttribute)
  {
    Rml::String value;
    if (!pDataModelAttribute->GetInto(value))
      return;

    dict.FindOrAdd(value.c_str(),nullptr);
  }

  for(int i = 0; i < pElement->GetNumChildren(false); ++i)
  {
    CollectDataModel(pElement->GetChild(i),dict);
  }
}

void ezRmlUiDocumentContext::HandleMessage(const ezEditorEngineDocumentMsg* pMsg)
{
  if (pMsg->GetDynamicRTTI()->IsDerivedFrom<ezEditorEngineForceRefreshMsg>())
  {
    ezRmlUiCanvas2DComponent* pComponent = nullptr;

    if(m_pMainObject->TryGetComponentOfBaseType<ezRmlUiCanvas2DComponent>(pComponent) && pComponent)
    {
      ezMsgRmlUiReload reloadMsg;
      pComponent->SendMessage(reloadMsg);
    }
  }
  else if (pMsg->GetDynamicRTTI()->IsDerivedFrom<ezEditorEngineAutoFillDataBindingMsg>())
  {
    ezLog::Error("Filling the context!");
    EZ_LOCK(m_pMainObject->GetWorld()->GetWriteMarker());

    ezRmlUiCanvas2DComponent* pComponent = nullptr;
    if(m_pMainObject->TryGetComponentOfBaseType<ezRmlUiCanvas2DComponent>(pComponent) && pComponent)
    {
      ezVariantDictionary dict;
      CollectDataModel(pComponent->GetOrCreateRmlContext()->GetRootElement(),dict);

      ezLog::Error("Found data models!!");
      for(const auto& entry : dict)
      {
        ezLog::Warning("{} : {}",entry.Key(),entry.Value().ConvertTo<ezString>());
      }


      ezInternal::NewInstance<MockDataBinding> pDataBinding = EZ_DEFAULT_NEW(MockDataBinding);
      pComponent->AddDataBinding(pDataBinding);
      ezLog::Error("Cool");


      //TEST Response msg to something
      ezEditorEngineAutoFillDataBindingMsg response;
      response.m_DocumentGuid = pMsg->m_DocumentGuid;
      response.m_Bindings.Insert("HP","123");
      //response.m_Bindings.Insert("MAXHP","123");

      for(const auto& entry : response.m_Bindings)
      {
        ezLog::Warning("{} : {}",entry.Key(),entry.Value().ConvertTo<ezString>());
      }

      //pMsg.m_uiViewID =
      SendProcessMessage(&response);
      ezLog::Error("Sent!!");
      return;
    }
    else
    {
      ezLog::Error("Failed!");
    }


    //RestartEffect();
    return;
  }

  ezEngineProcessDocumentContext::HandleMessage(pMsg);
}

void ezRmlUiDocumentContext::OnInitialize()
{
  auto pWorld = m_pWorld;
  EZ_LOCK(pWorld->GetWriteMarker());

  // Preview object
  {
    ezGameObjectDesc obj;
    obj.m_sName.Assign("RmlUiPreview");
    obj.m_bDynamic = true;
    pWorld->CreateObject(obj, m_pMainObject);

    ezRmlUiCanvas2DComponent* pComponent = nullptr;
    ezRmlUiCanvas2DComponent::CreateComponent(m_pMainObject, pComponent);

    ezStringBuilder sResourceGuid;
    ezConversionUtils::ToString(GetDocumentGuid(), sResourceGuid);
    m_hMainResource = ezResourceManager::LoadResource<ezRmlUiResource>(sResourceGuid);

    pComponent->SetRmlResource(m_hMainResource);
  }
}

ezEngineProcessViewContext* ezRmlUiDocumentContext::CreateViewContext()
{
  return EZ_DEFAULT_NEW(ezRmlUiViewContext, this);
}

void ezRmlUiDocumentContext::DestroyViewContext(ezEngineProcessViewContext* pContext)
{
  EZ_DEFAULT_DELETE(pContext);
}

bool ezRmlUiDocumentContext::UpdateThumbnailViewContext(ezEngineProcessViewContext* pThumbnailViewContext)
{
  EZ_LOCK(m_pMainObject->GetWorld()->GetWriteMarker());

  m_pMainObject->UpdateLocalBounds();
  ezBoundingBoxSphere bounds = m_pMainObject->GetGlobalBounds();

  ezRmlUiViewContext* pMeshViewContext = static_cast<ezRmlUiViewContext*>(pThumbnailViewContext);
  return pMeshViewContext->UpdateThumbnailCamera(bounds);
}
