#include "SharedPluginAssets/Common/Messages.h"
#include "SharedPluginRmlUi/Common/Messages.h"


#include <EditorPluginRmlUi/EditorPluginRmlUiPCH.h>
#include <QPushButton>
#include <QVBoxLayout>

#include <EditorFramework/DocumentWindow/EngineViewWidget.moc.h>
#include <EditorFramework/InputContexts/EditorInputContext.h>
#include <EditorPluginRmlUi/RmlUiAsset/RmlUiAssetWindow.moc.h>
#include <GuiFoundation/ActionViews/MenuBarActionMapView.moc.h>
#include <GuiFoundation/ActionViews/ToolBarActionMapView.moc.h>
#include <GuiFoundation/DockPanels/DocumentPanel.moc.h>
#include <GuiFoundation/PropertyGrid/PropertyGridWidget.moc.h>

ezQtRmlUiAssetDocumentWindow::ezQtRmlUiAssetDocumentWindow(ezAssetDocument* pDocument)
  : ezQtEngineDocumentWindow(pDocument)
{
  // Menu Bar
  {
    ezQtMenuBarActionMapView* pMenuBar = static_cast<ezQtMenuBarActionMapView*>(menuBar());
    ezActionContext context;
    context.m_sMapping = "RmlUiAssetMenuBar";
    context.m_pDocument = pDocument;
    context.m_pWindow = this;
    pMenuBar->SetActionContext(context);
  }

  // Tool Bar
  {
    ezQtToolBarActionMapView* pToolBar = new ezQtToolBarActionMapView("Toolbar", this);
    ezActionContext context;
    context.m_sMapping = "RmlUiAssetToolBar";
    context.m_pDocument = pDocument;
    context.m_pWindow = this;
    pToolBar->SetActionContext(context);
    pToolBar->setObjectName("RmlUiAssetWindowToolBar");
    addToolBar(pToolBar);
  }

  // 3D View
  ezQtViewWidgetContainer* pContainer = nullptr;
  {
    SetTargetFramerate(25);

    m_ViewConfig.m_Camera.LookAt(ezVec3(-1.6f, 0, 0), ezVec3(0, 0, 0), ezVec3(0, 0, 1));
    m_ViewConfig.ApplyPerspectiveSetting(90);

    m_pViewWidget = new ezQtEngineViewWidget(nullptr, this, &m_ViewConfig);
    AddViewWidget(m_pViewWidget);
    pContainer = new ezQtViewWidgetContainer(this, m_pViewWidget, "MeshAssetViewToolBar");
    setCentralWidget(pContainer);
  }

  // Property Grid
  {
    ezQtDocumentPanel* pPropertyPanel = new ezQtDocumentPanel(this, pDocument);
    pPropertyPanel->setObjectName("RmlUiAssetDockWidget");
    pPropertyPanel->setWindowTitle("RmlUi Properties");
    pPropertyPanel->show();

    ezQtPropertyGridWidget* pPropertyGrid = new ezQtPropertyGridWidget(pPropertyPanel, pDocument);
    pPropertyPanel->setWidget(pPropertyGrid);

    addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, pPropertyPanel);

    pDocument->GetSelectionManager()->SetSelection(pDocument->GetObjectManager()->GetRootObject()->GetChildren()[0]);
  }

  //RMLUI Test Utils
  {
    // Exposed Parameters
    if (true)
    {
      ezQtDocumentPanel* pPanel = new ezQtDocumentPanel(this, pDocument);
      pPanel->setObjectName("RmlUiAssetTestingDockWidget");
      pPanel->setWindowTitle("RmlUi Test Utilities");
      pPanel->show();

      //TODO Add a button here for "ForceRefresh"
      //Also add ability to create mock data for the current asset, which can be enabled/disabled ect for easy designing

      QWidget* pButtonGroup = new QWidget(pPanel);
      pButtonGroup->setLayout(new QVBoxLayout());


      m_pForceRefresh = new QPushButton(pButtonGroup);
      m_pForceRefresh->setText("Force refresh");
      m_pForceRefresh->setToolTip("Forces to reload the UI. Useful if not all file dependencies are picked up correctly.");
      connect(m_pForceRefresh, &QPushButton::clicked, this, &ezQtRmlUiAssetDocumentWindow::OnForceRefreshClicked);

      pButtonGroup->layout()->setContentsMargins(0, 0, 0, 0);
      pButtonGroup->layout()->addWidget(m_pForceRefresh);
      pButtonGroup->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));

      m_pAutoFill = new QPushButton(pButtonGroup);
      m_pAutoFill->setText("Auto Fill DataBindings");
      m_pAutoFill->setToolTip("Automatically fills in the below data binding properties");
      connect(m_pAutoFill, &QPushButton::clicked, this, &ezQtRmlUiAssetDocumentWindow::OnAutoFillClicked);
      pButtonGroup->layout()->addWidget(m_pAutoFill);

      pPanel->setWidget(pButtonGroup);

      /*ezQtPropertyGridWidget* pPropertyGrid = new ezQtPropertyGridWidget(pPanel, pDocument, false);
      ezDeque<const ezDocumentObject*> selection;
      selection.PushBack(pDocument->GetSettingsObject());
      pPropertyGrid->SetSelection(selection);
      pPanel->setWidget(pPropertyGrid);*/

      addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, pPanel);
    }
  }

  m_pAssetDoc = static_cast<ezRmlUiAssetDocument*>(pDocument);

  FinishWindowCreation();
}

void ezQtRmlUiAssetDocumentWindow::OnForceRefreshClicked(bool)
{
  ezEditorEngineForceRefreshMsg msg;
  GetEditorEngineConnection()->SendMessage(&msg);
}

void ezQtRmlUiAssetDocumentWindow::OnAutoFillClicked(bool)
{
  ezEditorEngineAutoFillDataBindingMsg msg;
  GetEditorEngineConnection()->SendMessage(&msg);
}

void ezQtRmlUiAssetDocumentWindow::InternalRedraw()
{
  ezEditorInputContext::UpdateActiveInputContext();
  SendRedrawMsg();
  ezQtEngineDocumentWindow::InternalRedraw();
}

void ezQtRmlUiAssetDocumentWindow::ProcessMessageEventHandler(const ezEditorEngineDocumentMsg* pMsg)
{
  //ezStringBuilder msg;
  //msg.Format("Received!");
  //ezQtUiServices::GetSingleton()->MessageBoxInformation(msg);

  if (pMsg->GetDynamicRTTI()->IsDerivedFrom<ezEditorEngineAutoFillDataBindingMsg>())
  {
    ezLog::Warning("Received, printing bindings!!");

    for(const auto& entry : static_cast<const ezEditorEngineAutoFillDataBindingMsg*>(pMsg)->m_Bindings)
    {
      ezLog::Warning("{} : {}",entry.Key(),entry.Value().ConvertTo<ezString>());
    }
    ezLog::Warning("Done!");

    //ezQtUiServices::GetSingleton()->MessageBoxInformation(msg);
    //SetAutoRestartEffect(((const ezEditorEngineLoopAnimationMsg*)pMsg)->m_bLoop);
  }

  ////
  ezQtEngineDocumentWindow::ProcessMessageEventHandler(pMsg);
}

void ezQtRmlUiAssetDocumentWindow::SendRedrawMsg()
{
  // do not try to redraw while the process is crashed, it is obviously futile
  if (ezEditorEngineProcessConnection::GetSingleton()->IsProcessCrashed())
    return;

  for (auto pView : m_ViewWidgets)
  {
    pView->SetEnablePicking(false);
    pView->UpdateCameraInterpolation();
    pView->SyncToEngine();
  }
}
