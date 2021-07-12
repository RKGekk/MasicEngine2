#pragma once

#include <memory>
#include <vector>

#include <DirectXMath.h>

#include "light_node.h"
#include "../graphics/constant_buffer_types.h"

#define MAXIMUM_LIGHTS_SUPPORTED (3)

class LightManager {
	friend class Scene;

protected:
	Lights m_Lights;
	std::vector<DirectionalLight> gDirLights;
	//PointLight gPointLights[3];
	//SpotLight gSpotLights[3];
	
public:
	void CalcLighting(Scene* pScene);
	void CopyLighting(CB_PS_PixelShader_Light* pLighting, SceneNode* pNode);
	int GetLightCount(const SceneNode* node);
};
