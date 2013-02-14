#include "SceneGraphCommands.h"

#include "DAVAEngine.h"
#include "../Qt/Scene/SceneDataManager.h"
#include "../Qt/Scene/SceneData.h"
#include "../EditorScene.h"
#include "Scene3D/Components/DebugRenderComponent.h"


using namespace DAVA;

CommandRemoveRootNodes::CommandRemoveRootNodes()
    :   Command(Command::COMMAND_UNDO_REDO)
    ,   activeScene(NULL)
{
}


void CommandRemoveRootNodes::Execute()
{
    activeScene = SceneDataManager::Instance()->SceneGetActive();
    SceneNode *selectedNode = activeScene->GetSelectedNode();
    EditorScene *scene = activeScene->GetScene();
    if(selectedNode && scene && (selectedNode->GetParent() == scene))
    {
        //TODO: save scene state here
        
        
        String referenceToOwner;
        
        KeyedArchive *customProperties = selectedNode->GetCustomProperties();
        if(customProperties && customProperties->IsKeyExists("editor.referenceToOwner"))
        {
            referenceToOwner = customProperties->GetString("editor.referenceToOwner");
        }
        
        
        Vector<SceneNode *>nodesForDeletion;
        nodesForDeletion.reserve(scene->GetChildrenCount());
        
        for(int32 i = 0; i < scene->GetChildrenCount(); ++i)
        {
            SceneNode *node = scene->GetChild(i);
            
            customProperties = node->GetCustomProperties();
            if(customProperties && customProperties->IsKeyExists("editor.referenceToOwner"))
            {
                if(customProperties->GetString("editor.referenceToOwner") == referenceToOwner)
                {
                    nodesForDeletion.push_back(SafeRetain(node));
                }
            }
        }
        
        scene->SetSelection(NULL);
        activeScene->SelectNode(NULL);
        
        for(int32 i = 0; i < (int32)nodesForDeletion.size(); ++i)
        {
            SceneNode *node = nodesForDeletion[i];
            
            scene->ReleaseUserData(node);
            scene->RemoveNode(node);
            
            SafeRelease(node);
        }
        nodesForDeletion.clear();
        
//        SceneValidator::Instance()->EnumerateSceneTextures();
        
        SceneData *sceneData = SceneDataManager::Instance()->SceneGetActive();
        sceneData->RebuildSceneGraph();
    }
    else
    {
        SetState(STATE_INVALID);
    }
}

void CommandRemoveRootNodes::Cancel()
{
    //TODO: restore saved state if active scene is same as saved
}


CommandRefreshSceneGraph::CommandRefreshSceneGraph()
    :   Command(Command::COMMAND_WITHOUT_UNDO_EFFECT)
{
}


void CommandRefreshSceneGraph::Execute()
{
    SceneData * activeScene = SceneDataManager::Instance()->SceneGetActive();
    activeScene->RebuildSceneGraph();
}


CommandLockAtObject::CommandLockAtObject()
:   Command(Command::COMMAND_WITHOUT_UNDO_EFFECT)
{
}


void CommandLockAtObject::Execute()
{
    SceneData * activeScene = SceneDataManager::Instance()->SceneGetActive();
    SceneNode *node = activeScene->GetSelectedNode();
    if(node)
    {
        activeScene->LockAtSelectedNode();
    }
    else
    {
        SetState(STATE_INVALID);
    }
}



CommandRemoveSceneNode::CommandRemoveSceneNode()
    :   Command(Command::COMMAND_UNDO_REDO)
    ,   activeScene(NULL)
	,	undoNode(NULL)
{
}

CommandRemoveSceneNode::~CommandRemoveSceneNode()
{
	SafeRelease(undoNode);
}

void CommandRemoveSceneNode::Execute()
{
	if (!undoNode)
	{
		activeScene = SceneDataManager::Instance()->SceneGetActive();
		SceneNode *node = activeScene->GetSelectedNode();
		if (!node)
		{
			SetState(STATE_INVALID);
			return;
		}

		undoNode = SafeRetain(node);
	}

	activeScene->RemoveSceneNode(undoNode);
}

void CommandRemoveSceneNode::Cancel()
{
	if (undoNode)
	{
		if (activeScene == SceneDataManager::Instance()->SceneGetActive())
		{
			activeScene->AddSceneNode(undoNode);
			activeScene->SelectNode(undoNode);
		}
	}
}


CommandDebugFlags::CommandDebugFlags()
    :   Command(Command::COMMAND_WITHOUT_UNDO_EFFECT)
{
}


void CommandDebugFlags::Execute()
{
    SceneData * activeScene = SceneDataManager::Instance()->SceneGetActive();
    SceneNode *node = activeScene->GetSelectedNode();
    if(node)
    {
        if (node->GetDebugFlags() & DebugRenderComponent::DEBUG_DRAW_ALL)
        {
            node->SetDebugFlags(0, true);
        }
        else
        {
            node->SetDebugFlags(DebugRenderComponent::DEBUG_DRAW_ALL, true);
        }
    }
    else
    {
        SetState(STATE_INVALID);
    }
}


