#pragma once
#include "Shader.h"

class SandShader : public Shader
{
public:
	SandShader();
	~SandShader();

	bool InitStandard(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
	bool SetShaderParameters(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* world,
		DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, Light* sceneLight1,
		ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* textureNormals,
		DirectX::SimpleMath::Vector3 cameraPos, ID3D11ShaderResourceView* heightTexture,
		ID3D11ShaderResourceView* wetSandTexture, ID3D11ShaderResourceView* wetSandNormalTexture);
};

