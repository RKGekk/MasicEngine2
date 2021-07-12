#include "scene.h"
#include "root_node.h"
#include "camera_node.h"
#include "../events/evt_data_new_render_component.h"
#include "../events/evt_data_destroy_actor.h"
#include "../events/evt_data_move_actor.h"
#include "../events/evt_data_modified_render_component.h"
#include "light_manager.h"
#include "../engine/engine.h"

void Scene::SetCamera(std::shared_ptr<CameraNode> camera) {
	m_Camera = camera;
}

const std::shared_ptr<CameraNode> Scene::GetCamera() const {
	return m_Camera;
}

void Scene::ActivateScene(bool isActive) {
	m_scene_active = isActive;
}

void Scene::PushAndSetMatrix4x4(const DirectX::XMFLOAT4X4& toWorld) {
	m_MatrixStack->Push();
	m_MatrixStack->MultMatrixLocal(toWorld);
	m_Renderer->VSetWorldTransform4x4(GetTopMatrix4x4f());
}

void Scene::PushAndSetMatrix(DirectX::FXMMATRIX toWorld) {
	m_MatrixStack->Push();
	m_MatrixStack->MultMatrixLocal(toWorld);
	m_Renderer->VSetWorldTransform4x4(GetTopMatrix4x4f());
}

void Scene::PopMatrix() {
	m_MatrixStack->Pop();
	m_Renderer->VSetWorldTransform4x4(GetTopMatrix4x4f());
}

DirectX::XMMATRIX Scene::GetTopMatrix() {
	return DirectX::XMLoadFloat4x4(&m_MatrixStack->GetTop());
}

const DirectX::XMFLOAT4X4& Scene::GetTopMatrix4x4f() {
	return m_MatrixStack->GetTop();
}

DirectX::XMFLOAT4X4 Scene::GetTopMatrix4x4fT() {
	DirectX::XMMATRIX t = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_MatrixStack->GetTop()));
	DirectX::XMFLOAT4X4 t4x4;
	DirectX::XMStoreFloat4x4(&t4x4, t);
	return t4x4;
}

DirectX::XMMATRIX Scene::GetTopInvMatrix() {
	return DirectX::XMMatrixInverse(nullptr, GetTopMatrix());
}

DirectX::XMFLOAT4X4 Scene::GetTopInvMatrix4x4f() {
	DirectX::XMMATRIX t = GetTopInvMatrix();
	DirectX::XMFLOAT4X4 t4x4;
	DirectX::XMStoreFloat4x4(&t4x4, t);
	return t4x4;
}

DirectX::XMFLOAT4X4 Scene::GetTopInvMatrix4x4fT() {
	DirectX::XMMATRIX t = DirectX::XMMatrixTranspose(GetTopInvMatrix());
	DirectX::XMFLOAT4X4 t4x4;
	DirectX::XMStoreFloat4x4(&t4x4, t);
	return t4x4;
}

LightManager* Scene::GetLightManager() {
	return m_LightManager.get();
}

void Scene::AddAlphaSceneNode(AlphaSceneNode* asn) {
	m_AlphaSceneNodes.push_back(asn);
}

HRESULT Scene::Pick(RayCast* pRayCast) {
	return m_Root->VPick(this, pRayCast);
}

IRenderer* Scene::GetRenderer() {
	return m_Renderer;
}

void Scene::RenderAlphaPass() {
	std::shared_ptr<IRenderState> alphaPass = m_Renderer->VPrepareAlphaPass();

	m_AlphaSceneNodes.sort();
	while (!m_AlphaSceneNodes.empty()) 	{
		AlphaSceneNodes::reverse_iterator i = m_AlphaSceneNodes.rbegin();
		PushAndSetMatrix4x4((*i)->m_Concat);
		(*i)->m_pNode->VRender(this);
		delete (*i);
		PopMatrix();
		m_AlphaSceneNodes.pop_back();
	}
}

Scene::Scene(IRenderer* renderer) {
	m_MatrixStack = std::make_shared<MatrixStack>();
	m_Root.reset(new RootNode());
	m_Renderer = renderer;
	m_LightManager = std::make_unique<LightManager>();

	IEventManager* pEventMgr = IEventManager::Get();
	pEventMgr->VAddListener({ connect_arg<&Scene::NewRenderComponentDelegate>, this }, EvtData_New_Render_Component::sk_EventType);
	pEventMgr->VAddListener({ connect_arg<&Scene::DestroyActorDelegate>, this }, EvtData_Destroy_Actor::sk_EventType);
	pEventMgr->VAddListener({ connect_arg<&Scene::MoveActorDelegate>, this }, EvtData_Move_Actor::sk_EventType);
	pEventMgr->VAddListener({ connect_arg<&Scene::ModifiedRenderComponentDelegate>, this }, EvtData_Modified_Render_Component::sk_EventType);
}

Scene::~Scene() {
	IEventManager* pEventMgr = IEventManager::Get();
	pEventMgr->VRemoveListener({ connect_arg<&Scene::NewRenderComponentDelegate>, this }, EvtData_New_Render_Component::sk_EventType);
	pEventMgr->VRemoveListener({ connect_arg<&Scene::DestroyActorDelegate>, this }, EvtData_Destroy_Actor::sk_EventType);
	pEventMgr->VRemoveListener({ connect_arg<&Scene::MoveActorDelegate>, this }, EvtData_Move_Actor::sk_EventType);
	pEventMgr->VRemoveListener({ connect_arg<&Scene::ModifiedRenderComponentDelegate>, this }, EvtData_Modified_Render_Component::sk_EventType);
}

HRESULT Scene::OnRender() {
	if (m_Root && m_Camera) {
		m_Camera->SetViewTransform(this);
		m_LightManager->CalcLighting(this);

		if (m_Root->VPreRender(this) == S_OK) {
			m_Root->VRender(this);
			m_Root->VRenderChildren(this);
			m_Root->VPostRender(this);
		}
		RenderAlphaPass();
	}

	return S_OK;
}

HRESULT Scene::OnRestore() {
	if (!m_Root) { return S_OK;	}

	HRESULT hr = m_Renderer->VOnRestore();
	if (FAILED(hr)) { return hr; };

	return m_Root->VOnRestore(this);
}

HRESULT Scene::OnLostDevice() {
	if (m_Root) {
		return m_Root->VOnLostDevice(this);
	}
	return S_OK;
}

HRESULT Scene::OnUpdate(float deltaSeconds) {
	if (!m_Root) {
		return S_OK;
	}
	return m_Root->VOnUpdate(this, g_pApp->GetTimer().DeltaTime());
}

std::shared_ptr<ISceneNode> Scene::FindActor(ActorId id) {
	SceneActorMap::iterator i = m_ActorMap.find(id);
	if (i == m_ActorMap.end()) 	{
		return std::shared_ptr<ISceneNode>();
	}

	return i->second;
}

bool Scene::AddChild(ActorId id, std::shared_ptr<ISceneNode> kid) {
	if (id != INVALID_ACTOR_ID) {
		m_ActorMap[id] = kid;
	}

	std::shared_ptr<LightNode> pLight = std::dynamic_pointer_cast<LightNode>(kid);
	if (pLight && m_LightManager->m_Lights.size() + 1 < MAXIMUM_LIGHTS_SUPPORTED) 	{
		m_LightManager->m_Lights.push_back(pLight);
	}
	return m_Root->VAddChild(kid);
}

bool Scene::RemoveChild(ActorId id) {
	if (id == INVALID_ACTOR_ID) {
		return false;
	}

	std::shared_ptr<ISceneNode> kid = FindActor(id);
	if(!kid) { return false; }

	std::shared_ptr<LightNode> pLight = std::dynamic_pointer_cast<LightNode>(kid);
	if (pLight) {
		m_LightManager->m_Lights.remove(pLight);
	}
	m_ActorMap.erase(id);
	return m_Root->VRemoveChild(id);
}

void Scene::NewRenderComponentDelegate(IEventDataPtr pEventData) {
	if(!m_scene_active) {return;}
	std::shared_ptr<EvtData_New_Render_Component> pCastEventData = std::static_pointer_cast<EvtData_New_Render_Component>(pEventData);

	ActorId actorId = pCastEventData->GetActorId();
	std::shared_ptr<SceneNode> pSceneNode(pCastEventData->GetSceneNode());

	if (FAILED(pSceneNode->VOnRestore(this))) {
		std::string error = "Failed to restore scene node to the scene for actorid " + std::to_string(actorId);
		return;
	}

	AddChild(actorId, pSceneNode);
}

void Scene::ModifiedRenderComponentDelegate(IEventDataPtr pEventData) {
	std::shared_ptr<EvtData_Modified_Render_Component> pCastEventData = std::static_pointer_cast<EvtData_Modified_Render_Component>(pEventData);

	ActorId actorId = pCastEventData->GetActorId();
	if (actorId == INVALID_ACTOR_ID) 	{
		return;
	}

	if (g_pApp->GetGameLogic()->GetState() == BaseEngineState::BGS_LoadingGameEnvironment) {
		return;
	}

	std::shared_ptr<ISceneNode> pSceneNode = FindActor(actorId);
	
	if (!pSceneNode || FAILED(pSceneNode->VOnRestore(this))) {
		std::string err = "Failed to restore scene node to the scene for actorid " + std::to_string(actorId);
	}
}

void Scene::DestroyActorDelegate(IEventDataPtr pEventData) {
	std::shared_ptr<EvtData_Destroy_Actor> pCastEventData = std::static_pointer_cast<EvtData_Destroy_Actor>(pEventData);
	RemoveChild(pCastEventData->GetId());
}

void Scene::MoveActorDelegate(IEventDataPtr pEventData) {
	std::shared_ptr<EvtData_Move_Actor> pCastEventData = std::static_pointer_cast<EvtData_Move_Actor>(pEventData);

	ActorId id = pCastEventData->GetId();
	DirectX::XMFLOAT4X4 transform = pCastEventData->GetMatrix4x4();

	std::shared_ptr<ISceneNode> pNode = FindActor(id);
	if (pNode) {
		pNode->VSetTransform4x4(&transform, nullptr);
	}
}
