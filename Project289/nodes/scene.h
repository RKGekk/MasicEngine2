#pragma once

#include <memory>

#include "i_scene_node.h"
#include "scene_node.h"
#include "../engine/i_renderer.h"
#include "matrix_stack.h"
#include "alpha_scene_node.h"

class CameraNode;
class SkyNode;
class LightNode;
class LightManager;
class RootNode;

class Scene {
protected:
	bool m_scene_active = true;

	std::shared_ptr<RootNode> m_Root;
	std::shared_ptr<CameraNode> m_Camera;
	IRenderer* m_Renderer;

	std::shared_ptr<MatrixStack> m_MatrixStack;
	AlphaSceneNodes m_AlphaSceneNodes;
	SceneActorMap m_ActorMap;

	std::unique_ptr<LightManager> m_LightManager;

	void RenderAlphaPass();

public:
	Scene(IRenderer* renderer);
	virtual ~Scene();

	HRESULT OnRender();
	HRESULT OnRestore();
	HRESULT OnLostDevice();
	HRESULT OnUpdate(float deltaSeconds);
	std::shared_ptr<ISceneNode> FindActor(ActorId id);
	bool AddChild(ActorId id, std::shared_ptr<ISceneNode> kid);
	bool RemoveChild(ActorId id);

	void NewRenderComponentDelegate(IEventDataPtr pEventData);
	void ModifiedRenderComponentDelegate(IEventDataPtr pEventData);
	void DestroyActorDelegate(IEventDataPtr pEventData);
	void MoveActorDelegate(IEventDataPtr pEventData);

	void SetCamera(std::shared_ptr<CameraNode> camera);
	const std::shared_ptr<CameraNode> GetCamera() const;

	void ActivateScene(bool isActive);

	void PushAndSetMatrix4x4(const DirectX::XMFLOAT4X4& toWorld);
	void PushAndSetMatrix(DirectX::FXMMATRIX toWorld);
	void PopMatrix();
	DirectX::XMMATRIX GetTopMatrix();
	const DirectX::XMFLOAT4X4& GetTopMatrix4x4f();
	DirectX::XMFLOAT4X4 GetTopMatrix4x4fT();
	DirectX::XMMATRIX GetTopInvMatrix();
	DirectX::XMFLOAT4X4 GetTopInvMatrix4x4f();
	DirectX::XMFLOAT4X4 GetTopInvMatrix4x4fT();

	LightManager* GetLightManager();

	void AddAlphaSceneNode(AlphaSceneNode* asn);

	HRESULT Pick(RayCast* pRayCast);

	IRenderer* GetRenderer();
};