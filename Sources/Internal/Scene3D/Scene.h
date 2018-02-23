#pragma once

#include "Asset/Asset.h"
#include "Base/BaseMath.h"
#include "Base/BaseTypes.h"
#include "Base/Observer.h"
#include "Entity/SceneSystem.h"
#include "Entity/SingletonComponent.h"
#include "Entity/SystemManager.h"
#include "Reflection/ReflectedTypeDB.h"
#include "Reflection/Reflection.h"
#include "Render/Highlevel/Camera.h"
#include "Render/RenderBase.h"
#include "Scene3D/ComponentGroup.h"
#include "Scene3D/Entity.h"
#include "Scene3D/EntityGroup.h"
#include "Scene3D/Prefab.h"
#include "Scene3D/SceneFile/SerializationContext.h"
#include "Scene3D/SceneFile/VersionInfo.h"
#include "Scene3D/SceneFileV2.h"

namespace DAVA
{
/**
    \defgroup scene3d 3D Engine
  */

class Texture;
class DataNode;
class ShadowVolumeNode;
class Light;
class ShadowRect;
class QuadTree;
class Component;
class RenderSystem;
class RenderUpdateSystem;
class TransformSystem;
class DebugRenderSystem;
class EventSystem;
class ParticleEffectSystem;
class UpdateSystem;
class LightUpdateSystem;
class SwitchSystem;
class SoundUpdateSystem;
class ActionUpdateSystem;
class StaticOcclusionSystem;
class StaticOcclusionDebugDrawSystem;
class SpeedTreeUpdateSystem;
class FoliageSystem;
class WindSystem;
class WaveSystem;
class SkeletonSystem;
class MotionSystem;
class AnimationSystem;
class LandscapeSystem;
class LodSystem;
class PostEffectSystem;
class PostEffectDebugSystem;
class ReflectionSystem;
class LightRenderSystem;
class DecalSystem;
class VTDecalSystem;
class VTSplineDecalSystem;
class ParticleEffectDebugDrawSystem;
class GeoDecalSystem;
class SlotSystem;
class EnvironmentSystem;
class TransformSingleComponent;
class ActionsSingleComponent;
class ActionCollectSystem;
class DiffMonitoringSystem;
class MotionSingleComponent;
class VTSingleComponent;
class PhysicsSystem;
class CollisionSingleComponent;
class StreamingSystem;
class EntitiesManager;

class UIEvent;
class RenderPass;

/**
    \ingroup scene3d
    \brief This class is a code of our 3D Engine scene graph. 
    To visualize any 3d scene you'll need to create Scene object. 
    Scene have visible hierarchy and invisible root nodes. You can add as many root nodes as you want, and do not visualize them.
    For example you can have multiple scenes, load them to one scene, and show each scene when it will be required. 
 */
class EntityCache
{
public:
    ~EntityCache();

    void Preload(const FilePath& path);
    void Clear(const FilePath& path);
    void ClearAll();

    Entity* GetOriginal(const FilePath& path);
    Entity* GetClone(const FilePath& path);

protected:
    Map<FilePath, Entity*> cachedEntities;
};

class Scene : public Entity, Observer
{
protected:
    virtual ~Scene();

public:
    enum class SceneSystemsPolicy
    {
        DefaultSystems,
        WithoutSystems,
    };

    Scene();
    Scene(SceneSystemsPolicy policy);
    Scene(const UnorderedSet<FastName>& tags);

    /**
        \brief Function to register entity in scene. This function is called when you add entity to scene.
     */
    void RegisterEntity(Entity* entity);
    /**
        \brief Function to unregister entity from scene. This function is called when you remove entity from scene.
     */
    void UnregisterEntity(Entity* entity);

    /**
        \brief Function to register component in scene. This function is called when you add any component to any entity in scene.
     */
    void RegisterComponent(Entity* entity, Component* component);
    /**
        \brief Function to unregister component from scene. This function is called when you remove any component from any entity in scene.
     */
    void UnregisterComponent(Entity* entity, Component* component);

    virtual void AddSystem(SceneSystem* sceneSystem, SceneSystem* insertBeforeSceneForProcess = nullptr, SceneSystem* insertBeforeSceneForInput = nullptr, SceneSystem* insertBeforeSceneForFixedProcess = nullptr);

    virtual void RemoveSystem(SceneSystem* sceneSystem);

    template <class T>
    T* GetSystem();

    /** Take effect on the next frame. Add tag to scene. All affected systems will be added automatically. */
    void AddTag(FastName tag);

    /** Take effect on the next frame. Remove tag from scene. All affected system will be removed automatically. */
    void RemoveTag(FastName tag);

    bool HasTag(FastName tag) const;

    template <typename Return, typename Cls>
    void RegisterSystemProcess(Return (Cls::*fp)(float32));

    Vector<SceneSystem*> systemsToProcess;
    Vector<SceneSystem*> systemsToInput;
    Vector<SceneSystem*> systemsToFixedProcess;

    StreamingSystem* streamingSystem = nullptr;
    TransformSystem* transformSystem = nullptr;
    RenderUpdateSystem* renderUpdateSystem = nullptr;
    LodSystem* lodSystem = nullptr;
    DebugRenderSystem* debugRenderSystem = nullptr;
    EventSystem* eventSystem = nullptr;
    ParticleEffectSystem* particleEffectSystem = nullptr;
    UpdateSystem* updatableSystem = nullptr;
    LightUpdateSystem* lightUpdateSystem = nullptr;
    SwitchSystem* switchSystem = nullptr;
    RenderSystem* renderSystem = nullptr;
    SoundUpdateSystem* soundSystem = nullptr;
    ActionUpdateSystem* actionSystem = nullptr;
    StaticOcclusionSystem* staticOcclusionSystem = nullptr;
    SpeedTreeUpdateSystem* speedTreeUpdateSystem = nullptr;
    FoliageSystem* foliageSystem = nullptr;
    VersionInfo::SceneVersion version;
    WindSystem* windSystem = nullptr;
    WaveSystem* waveSystem = nullptr;
    AnimationSystem* animationSystem = nullptr;
    StaticOcclusionDebugDrawSystem* staticOcclusionDebugDrawSystem = nullptr;
    SkeletonSystem* skeletonSystem = nullptr;
    MotionSystem* motionSystem = nullptr;
    LandscapeSystem* landscapeSystem = nullptr;
    PostEffectSystem* postEffectSystem = nullptr;
    PostEffectDebugSystem* postEffectDebugSystem = nullptr;
    ReflectionSystem* reflectionSystem = nullptr;
    LightRenderSystem* lightRenderSystem = nullptr;
    DecalSystem* decalSystem = nullptr;
    VTDecalSystem* vtDecalSystem = nullptr;
    VTSplineDecalSystem* vtSplineDecalSystem = nullptr;
    ParticleEffectDebugDrawSystem* particleEffectDebugDrawSystem = nullptr;
    SlotSystem* slotSystem = nullptr;
    ActionCollectSystem* actionCollectSystem;
    GeoDecalSystem* geoDecalSystem = nullptr;
    DiffMonitoringSystem* diffMonitoringSystem = nullptr;
    PhysicsSystem* physicsSystem = nullptr;
    EnvironmentSystem* environmentSystem = nullptr;

    /** Get singleton component. Never return nullptr. */
    template <class T>
    T* GetSingletonComponent();

    /** Get singleton component. Never return nullptr. */
    SingletonComponent* GetSingletonComponent(const Type* type);

    Vector<std::pair<SingletonComponent*, const Type*>> singletonComponents;
    Map<SingletonComponent*, size_t> singletonComponentsMap;

    /**
        \brief Overloaded GetScene returns this, instead of normal functionality.
     */
    Scene* GetScene() override;

    void HandleEvent(Observable* observable) override; //Handle RenderOptions

    //virtual void StopAllAnimations(bool recursive = true);

    virtual void Update(float32 timeElapsed);
    virtual void Draw();
    void SceneDidLoaded() override;

    Camera* GetCamera(int32 n);
    void AddCamera(Camera* c);
    bool RemoveCamera(Camera* c);
    inline int32 GetCameraCount();

    void SetCurrentCamera(Camera* camera);
    Camera* GetCurrentCamera() const;

    /* 
        This camera is used for visualization setup only. Most system functions use mainCamere, draw camera is used to setup matrices for render. If you do not call this function GetDrawCamera returns currentCamera. 
        You can use SetCustomDrawCamera function if you want to test frustum clipping, and view the scene from different angles.
     */
    void SetCustomDrawCamera(Camera* camera);
    Camera* GetDrawCamera() const;

    void CreateSystems();

    EventSystem* GetEventSystem() const;
    RenderSystem* GetRenderSystem() const;
    AnimationSystem* GetAnimationSystem() const;
    ParticleEffectDebugDrawSystem* GetParticleEffectDebugDrawSystem() const;
#if defined(__DAVAENGINE_PHYSICS_ENABLED__)
    PhysicsSystem* GetPhysicsSystem() const;
#endif

    virtual SceneFileV2::eError LoadScene(const DAVA::FilePath& pathname);
    virtual SceneFileV2::eError SaveScene(const DAVA::FilePath& pathname, bool saveForGame = false);

    void ConstructFromPrefab(const Asset<Prefab>& prefab);

    virtual void OptimizeBeforeExport();

    DAVA::NMaterial* GetGlobalMaterial() const;
    void SetGlobalMaterial(DAVA::NMaterial* globalMaterial);

    void OnSceneReady(Entity* rootNode);

    void Input(UIEvent* event);
    void InputCancelled(UIEvent* event);

    /**
        \brief This functions activate and deactivate scene systems
     */
    virtual void Activate();
    virtual void Deactivate();

    EntityCache cache;

    void SetMainPassProperties(uint32 priority, const Rect& viewport, uint32 width, uint32 height, PixelFormat format);
    void SetMainRenderTarget(rhi::HTexture color, rhi::HTexture depthStencil, bool loadContent, const Color& clearColor);

    void SetFixedUpdateTime(float32 time);
    void SetConstantUpdateTime(float32 time);

    void SetPerformFixedProcessOnlyOnce(bool isPerformFixedProcessOnlyOnce);

    void PauseFixedUpdate();
    void UnpauseFixedUpdate();
    bool IsFixedUpdatePaused() const;
    void CreateSystemsByTags();
    Vector<SceneSystem*> systemsVector;

    template <class... Args>
    EntityGroup* AquireEntityGroup();
    template <class Matcher, class... Args>
    EntityGroup* AquireEntityGroupWithMatcher();

    template <class T, class... Args>
    ComponentGroup<T>* AquireComponentGroup();
    template <class Matcher, class T, class... Args>
    ComponentGroup<T>* AquireComponentGroupWithMatcher();

    float GetGlobalTime() const;

protected:
    void RegisterEntitiesInSystemRecursively(SceneSystem* system, Entity* entity);

    bool RemoveSystem(Vector<SceneSystem*>& storage, SceneSystem* system);

    uint32 maxEntityIDCounter;
    float32 sceneGlobalTime = 0.f;

    Vector<Camera*> cameras;

    NMaterial* sceneGlobalMaterial;

    Camera* mainCamera;
    Camera* drawCamera;

    struct FixedUpdate
    {
        float32 constantTime = 1.f / 60.f;
        float32 fixedTime = 1.f / 60.f;
        float32 lastTime = 0.f;
    } fixedUpdate;
    bool pauseFixedUpdate = false;

    bool isPerformFixedProcessOnlyOnce = false;

    friend class Entity;
    DAVA_VIRTUAL_REFLECTION(Scene, Entity);

private:
    void ProcessManuallyAddedSystems(float32 timeElapsed);
    void ProcessSystemsAddedByTags(float32 timeElapsed);
    void CreateSystemsToMethods(const Vector<SystemManager::SceneProcessInfo>& methods);

    void InitLegacyPointers();
    void ProcessChangedTags();

    template <typename T>
    void AddSingletonComponent(T* component);
    void AddSingletonComponent(SingletonComponent* component, const Type* type);
    void RemoveSingletonComponent(SingletonComponent* component);

    UnorderedSet<FastName> tags;

    enum TagAction
    {
        ADD,
        REMOVE
    };
    Vector<std::pair<FastName, TagAction>> tagsToChange;

    UnorderedMap<const Type*, SceneSystem*> systemsMap;

    Vector<Function<void(float32)>> systemProcesses;

    EntitiesManager* entitiesManager = nullptr;
};
}

#include "Scene3D/Private/Scene.hpp"
