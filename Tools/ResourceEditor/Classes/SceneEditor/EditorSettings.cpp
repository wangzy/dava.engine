#include "EditorSettings.h"

EditorSettings::EditorSettings()
{
    settings = new KeyedArchive();
    
    settings->Load("~doc:/ResourceEditorOptions.archive");
}
    
EditorSettings::~EditorSettings()
{
    SafeRelease(settings);
}


KeyedArchive *EditorSettings::GetSettings()
{
    return settings;
}


void EditorSettings::Save()
{
    settings->Save("~doc:/ResourceEditorOptions.archive");
}


String EditorSettings::GetProjectPath()
{
    String projectPath = settings->GetString("LastSavedPath", "/");
    if('/' != projectPath[projectPath.length() - 1])
    {
        projectPath += '/';
    }
 
    return projectPath;
}