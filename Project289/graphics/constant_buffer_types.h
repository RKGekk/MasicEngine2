#pragma once

#include <DirectXMath.h>

#include "directional_light.h"
#include "point_light.h"
#include "spot_light.h"
#include "material_shader.h"

struct CB_VS_VertexShader {
	DirectX::XMFLOAT4X4 lwvpMatrix;
	DirectX::XMFLOAT4X4 invWorldMatrix;
	DirectX::XMFLOAT4X4 worldMatrix;
	MaterialShader material;
};

struct CB_PS_PixelShader_Light {
	DirectionalLight gDirLights[3];
	//PointLight gPointLights[3];
	//SpotLight gSpotLights[3];

	DirectX::XMFLOAT3 gEyePosW;
	float Pad;

	DirectX::XMFLOAT4 gFogColor;
	DirectX::XMFLOAT4 gFogStartAndRange;
};