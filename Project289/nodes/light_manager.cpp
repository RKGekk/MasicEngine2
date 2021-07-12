#include "light_manager.h"
#include "scene.h"

void LightManager::CalcLighting(Scene* pScene) {
	pScene->GetRenderer()->VCalcLighting(&m_Lights, MAXIMUM_LIGHTS_SUPPORTED);

	gDirLights.reserve(MAXIMUM_LIGHTS_SUPPORTED);


	int count = 0;
	int directional_count = 0;
	int point_count = 0;
	int spot_count = 0;
	for (Lights::iterator i = m_Lights.begin(); i != m_Lights.end(); ++i, ++count) 	{
		std::shared_ptr<LightNode> light = *i;

		const LightProperties& props = light->VGetLight();
		switch (props.m_LightType) 	{
			case LightType::DIRECTIONAL: {
				DirectionalLight dl;
				dl.Ambient = props.m_Ambient;
				dl.Diffuse = props.m_Diffuse;
				dl.Specular = props.m_Specular;
				dl.Direction = light->GetDirection();
				gDirLights.push_back(dl);
				//gDirLights[++directional_count] = dl;
			}
			break;
			case LightType::POINT: {
				//gPointLights[point_count].Ambient = props.m_Ambient;
				//gPointLights[point_count].Diffuse = props.m_Diffuse;
				//gPointLights[point_count].Specular = props.m_Specular;
				//gPointLights[point_count].Position = light->GetPosition3();
				//gPointLights[point_count].Att.x = props.m_Attenuation[0];
				//gPointLights[point_count].Att.y = props.m_Attenuation[1];
				//gPointLights[point_count].Att.z = props.m_Attenuation[2];
				//gPointLights[point_count].Range = props.m_Range;
				//++point_count;
			}
			break;
			case LightType::SPOT: {
				//gSpotLights[spot_count].Ambient = props.m_Ambient;
				//gSpotLights[spot_count].Diffuse = props.m_Diffuse;
				//gSpotLights[spot_count].Specular = props.m_Specular;
				//gSpotLights[spot_count].Position = light->GetPosition3();
				//gSpotLights[spot_count].Att.x = props.m_Attenuation[0];
				//gSpotLights[spot_count].Att.y = props.m_Attenuation[1];
				//gSpotLights[spot_count].Att.z = props.m_Attenuation[2];
				//gSpotLights[spot_count].Range = props.m_Range;
				//gSpotLights[spot_count].Direction = light->GetDirection();
				//gSpotLights[spot_count].Spot = props.m_Spot;
				//++spot_count;
			}
			break;
		}
	}
}

void LightManager::CopyLighting(CB_PS_PixelShader_Light* pLighting, SceneNode* pNode) {
	int count = GetLightCount(pNode);
	if (count) 	{
		for (size_t i = 0; i < gDirLights.size(); ++i) {
			pLighting->gDirLights[i] = gDirLights[i];
		}
	}
}

int LightManager::GetLightCount(const SceneNode* node) {
    return m_Lights.size();
}
