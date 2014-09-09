/*==================================================================================
    Copyright (c) 2014, thorin
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the binaryzebra nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/



#include "Scene3D/Systems/AnimationSystem.h"
#include "Scene3D/Components/AnimationComponent.h"
#include "Scene3D/Entity.h"
#include "Debug/DVAssert.h"
#include "Scene3D/Systems/EventSystem.h"
#include "Scene3D/Scene.h"
#include "Scene3D/Systems/GlobalEventSystem.h"
#include "Debug/Stats.h"
#include "Scene3D/SceneNodeAnimation.h"

namespace DAVA
{

AnimationSystem::AnimationSystem(Scene * scene)
:	SceneSystem(scene)
{
// 	scene->GetEventSystem()->RegisterSystemForEvent(this, EventSystem::LOCAL_TRANSFORM_CHANGED);
// 	scene->GetEventSystem()->RegisterSystemForEvent(this, EventSystem::TRANSFORM_PARENT_CHANGED);
}

AnimationSystem::~AnimationSystem()
{
}

void AnimationSystem::Process(float32 timeElapsed)
{
    TIME_PROFILE("TransformSystem::Process");
    Vector<AnimationComponent*>::iterator it, endit;
    for (it = items.begin(), endit = items.end(); it!= endit; ++it)
    {
        AnimationComponent * comp = *it;
        if (comp->isPlaying && comp->activeClip)
        {
            comp->time += timeElapsed;

            DAVA::Matrix3 rotationMatrix;
            DAVA::Matrix3 resultMatrix;
            SceneNodeAnimationKey & key = comp->activeClip->Intepolate(comp->time);
            Matrix4 result(key.rotation.GetMatrix() * comp->originalMatrix);
            result.SetTranslationVector(comp->originalTranslate + key.translation);
            comp->GetEntity()->SetLocalTransform(result);
        }
    }
}

void AnimationSystem::ImmediateEvent(Entity * entity, uint32 event)
{
	switch(event)
	{
	case EventSystem::LOCAL_TRANSFORM_CHANGED:
	case EventSystem::TRANSFORM_PARENT_CHANGED:
// 		EntityNeedUpdate(entity);
// 		HierahicAddToUpdate(entity);
		break;
	}
}


void AnimationSystem::AddEntity(Entity * entity)
{
// 	TransformComponent * transform = (TransformComponent*)entity->GetComponent(Component::TRANSFORM_COMPONENT);
// 	if (!transform) return; //just in case
// 	if(transform->parentMatrix)	
// 		transform->worldMatrix = transform->localMatrix * *(transform->parentMatrix);
// 	else
// 		transform->worldMatrix = transform->localMatrix;
}

void AnimationSystem::RemoveEntity(Entity * entity)
{
	//TODO: use hashmap
// 	uint32 size = updatableEntities.size();
// 	for(uint32 i = 0; i < size; ++i)
// 	{
// 		if(updatableEntities[i] == entity)
// 		{
// 			entity->RemoveFlag(Entity::TRANSFORM_NEED_UPDATE);
// 			entity->RemoveFlag(Entity::TRANSFORM_DIRTY);
// 
// 			updatableEntities[i] = updatableEntities[size-1];
// 			updatableEntities.pop_back();
// 			return;
// 		}
// 	}
}

};
