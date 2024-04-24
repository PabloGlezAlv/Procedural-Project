#include "pch.h"
#include "TerrainShader.h"

TerrainShader::TerrainShader() : Shader()
{
}

TerrainShader::~TerrainShader()
{
}

bool TerrainShader::InitStandard(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename)
{
	Shader::InitStandard(device, vsFilename, psFilename);
	return false;
}

bool TerrainShader::SetShaderParameters(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* world, 
	DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, Light* sceneLight1, 
	ID3D11ShaderResourceView* sandTexture, ID3D11ShaderResourceView* sandTextureNormals,
	ID3D11ShaderResourceView* grassTexture, ID3D11ShaderResourceView* grassTextureNormals,
	ID3D11ShaderResourceView* snowTexture, ID3D11ShaderResourceView* snowTextureNormals,
	ID3D11ShaderResourceView* rockTexture, ID3D11ShaderResourceView* rockTextureNormals, DirectX::SimpleMath::Vector3 cameraPos)
{
	Shader::SetShaderParameters(context, world, view, projection, sceneLight1, sandTexture, sandTextureNormals, cameraPos);

	context->PSSetShaderResources(3, 1, &snowTexture);
	context->PSSetShaderResources(4, 1, &snowTextureNormals);
	context->PSSetShaderResources(5, 1, &grassTexture);
	context->PSSetShaderResources(6, 1, &grassTextureNormals);
	context->PSSetShaderResources(7, 1, &rockTexture);
	context->PSSetShaderResources(8, 1, &rockTextureNormals);

	return false;
}
