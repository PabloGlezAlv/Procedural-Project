#pragma once
#include <chrono>

#include "DeviceResources.h"
#include "Light.h"

//Class from which we create all shader objects used by the framework
//This single class can be expanded to accomodate shaders of all different types with different parameters
class Shader
{
public:
	Shader();
	~Shader();

	//we could extend this to load in only a vertex shader, only a pixel shader etc.  or specialised init for Geometry or domain shader. 
	//All the methods here simply create new versions corresponding to your needs
	virtual bool InitStandard(ID3D11Device * device, WCHAR * vsFilename, WCHAR * psFilename);		//Loads the Vert / pixel Shader pair
	
	virtual bool SetShaderParameters(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, 
		Light* sceneLight1, ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* textureNormals,
		 DirectX::SimpleMath::Vector3 camPos);
	void EnableShader(ID3D11DeviceContext * context);

	void setFogParameters(float start, float end, float belowWater);

	void setShadowView(ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture,
		DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view,
		DirectX::SimpleMath::Matrix* projection, DirectX::SimpleMath::Matrix* lightView);


	float getSecondsSinceCreation();
protected:
	//standard matrix buffer supplied to all shaders
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX lightView;
		DirectX::XMMATRIX lightProjection;
		bool heightMap;
	};

	//buffer for information of a single light
	struct LightBufferType
	{
		DirectX::SimpleMath::Vector4 ambient;
		DirectX::SimpleMath::Vector4 diffuse;
		DirectX::SimpleMath::Vector3 position;
		float padding;
	};

	//buffer with fog Parameters
	struct FogBufferType
	{
		float startFog;
		float endFog;
		float underWater;
	};

	//buffer to pass in camera world Position
	struct CameraBufferType
	{
		DirectX::SimpleMath::Vector3 cameraPosition;
	};

	//Shaders
	Microsoft::WRL::ComPtr<ID3D11VertexShader>								m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>								m_pixelShader;

	ID3D11InputLayout*														m_layout;

	ID3D11SamplerState* m_clampState;
	ID3D11SamplerState* m_wrapState;

	ID3D11Buffer*															m_matrixBuffer;
	ID3D11Buffer*															m_cameraBuffer;
	ID3D11Buffer*															m_lightBuffer;
	ID3D11Buffer*															m_FogBuffer;
	//Time
	std::chrono::high_resolution_clock::time_point start;


	static float startFog;
	static float endFog;
	static float underWater;
};

