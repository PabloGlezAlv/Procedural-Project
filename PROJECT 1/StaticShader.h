#pragma once
#include "Shader.h"

class StaticShader : public Shader
{
public:
	StaticShader();
	~StaticShader();

	bool InitStandard(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);		
	bool SetShaderParameters(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* world,
		DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, Light* sceneLight1, 
		ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* textureNormals, 
		DirectX::SimpleMath::Vector3 cameraPos);


private:
	struct WaterBufferType
	{
		float time;
		float amplitude;
		float frequency;
	};

	ID3D11Buffer* m_waterBuffer;
};

