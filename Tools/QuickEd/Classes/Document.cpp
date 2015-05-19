#include "Document.h"
#include <QLineEdit>
#include <QAction>
#include <QItemSelection>
#include "UI/Preview/EditScreen.h"

#include "UI/Package/PackageModel.h"

#include "UI/Library/LibraryModel.h"

#include "Model/PackageHierarchy/PackageNode.h"
#include "Model/PackageHierarchy/PackageControlsNode.h"
#include "Model/PackageHierarchy/ControlNode.h"
#include "Model/PackageHierarchy/PackageRef.h"

#include "Model/ControlProperties/RootProperty.h"
#include "Model/ControlProperties/SectionProperty.h"
#include "Model/ControlProperties/ValueProperty.h"
#include "Model/ControlProperties/LocalizedTextValueProperty.h"
#include "Model/ControlProperties/FontValueProperty.h"

#include "SharedData.h"

#include "Ui/QtModelPackageCommandExecutor.h"
#include "EditorCore.h"

using namespace DAVA;

Document::Document(PackageNode *_package, QObject *parent)
    : QObject(parent)
    , package(SafeRetain(_package))
    , sharedData(new SharedData(this))
    , commandExecutor(new QtModelPackageCommandExecutor(this))
    , undoStack(new QUndoStack(this))
{
    InitSharedData();
    connect(sharedData, &SharedData::DataChanged, this, &Document::SharedDataChanged);
    connect(GetEditorFontSystem(), &EditorFontSystem::UpdateFontPreset, this, &Document::UpdateFonts);
}

void Document::InitSharedData()
{
    sharedData->SetData("controlDeselected", false);
    sharedData->SetData("controlsDeselected", false);

    QList<ControlNode*> rootControls;
    PackageControlsNode *controlsNode = package->GetPackageControlsNode();
    for (int32 index = 0; index < controlsNode->GetCount(); ++index)
        rootControls.push_back(controlsNode->Get(index));

    sharedData->SetData("activeRootControls", QVariant::fromValue(rootControls));
}

Document::~Document()
{
    SafeRelease(package);

    SafeRelease(commandExecutor);
}

const DAVA::FilePath &Document::GetPackageFilePath() const
{
    return package->GetPackageRef()->GetPath();
}

void Document::UpdateLanguage()
{
    package->GetPackageControlsNode()->RefreshControlProperties();
}

void Document::UpdateFonts()
{
    package->GetPackageControlsNode()->RefreshControlProperties();
}

