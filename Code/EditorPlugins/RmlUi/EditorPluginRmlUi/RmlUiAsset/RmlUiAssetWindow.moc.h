#pragma once

#include <EditorEngineProcessFramework/EngineProcess/ViewRenderSettings.h>
#include <EditorFramework/DocumentWindow/EngineDocumentWindow.moc.h>
#include <EditorPluginRmlUi/RmlUiAsset/RmlUiAsset.h>
#include <ToolsFoundation/Object/DocumentObjectManager.h>

class ezQtEngineViewWidget;

class ezQtRmlUiAssetDocumentWindow : public ezQtEngineDocumentWindow
{
  Q_OBJECT

public:
  ezQtRmlUiAssetDocumentWindow(ezAssetDocument* pDocument);

  virtual const char* GetWindowLayoutGroupName() const override { return "RmlUiAsset"; }

protected:
  virtual void InternalRedraw() override;

  virtual void ProcessMessageEventHandler(const ezEditorEngineDocumentMsg* pMsg) override;

private Q_SLOTS:
    void OnForceRefreshClicked(bool);
    void OnAutoFillClicked(bool);

private:
  void SendRedrawMsg();

  ezEngineViewConfig m_ViewConfig;
  ezQtEngineViewWidget* m_pViewWidget;
  ezRmlUiAssetDocument* m_pAssetDoc;

  QPushButton* m_pForceRefresh = nullptr;
  QPushButton* m_pAutoFill = nullptr;
};
