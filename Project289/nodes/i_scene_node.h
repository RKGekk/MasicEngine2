#pragma once

#include <memory>
#include <unordered_map>

#include <Windows.h>

#include "scene_node_properties.h"
#include "ray_cast.h"
#include "../actors/actor.h"

class Scene;
class ISceneNode;
typedef std::unordered_map<ActorId, std::shared_ptr<ISceneNode>> SceneActorMap;

class ISceneNode {
public:
	virtual const SceneNodeProperties& VGet() const = 0;

	virtual void VSetTransform(DirectX::FXMMATRIX toWorld, DirectX::CXMMATRIX fromWorld, bool calulate_from) = 0;
	virtual void VSetTransform4x4(const DirectX::XMFLOAT4X4* toWorld, const DirectX::XMFLOAT4X4* fromWorld) = 0;

	virtual HRESULT VOnUpdate(Scene* pScene, float elapsedSeconds) = 0;
	virtual HRESULT VOnRestore(Scene* pScene) = 0;

	virtual HRESULT VPreRender(Scene* pScene) = 0;
	virtual bool VIsVisible(Scene* pScene) const = 0;
	virtual HRESULT VRender(Scene* pScene) = 0;
	virtual HRESULT VRenderChildren(Scene* pScene) = 0;
	virtual HRESULT VPostRender(Scene* pScene) = 0;

	virtual bool VAddChild(std::shared_ptr<ISceneNode> kid) = 0;
	virtual bool VRemoveChild(ActorId id) = 0;
	virtual HRESULT VOnLostDevice(Scene* pScene) = 0;
	virtual HRESULT VPick(Scene* pScene, RayCast* pRayCast) = 0;

	virtual ISceneNode* VGetParent() = 0;

	virtual ~ISceneNode() {};
};