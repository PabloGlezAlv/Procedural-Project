#pragma once
#include "Shader.h"

class TerrainShader : public Shader
{
public:
	TerrainShader();
	~TerrainShader();

	bool InitStandard(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
	bool SetShaderParameters(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* world,
		DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, Light* sceneLight1,
		ID3D11ShaderResourceView* sandTexture, ID3D11ShaderResourceView* sandTextureNormals,
		ID3D11ShaderResourceView* grassTexture, ID3D11ShaderResourceView* grassTextureNormals,
		ID3D11ShaderResourceView* snowTexture, ID3D11ShaderResourceView* snowTextureNormals,
		ID3D11ShaderResourceView* rockTexture, ID3D11ShaderResourceView* rockTextureNormals,
		DirectX::SimpleMath::Vector3 cameraPos);
};

