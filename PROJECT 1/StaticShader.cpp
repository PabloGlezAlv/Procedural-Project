#include "pch.h"
#include "StaticShader.h"

StaticShader::StaticShader(): Shader()
{
}


StaticShader::~StaticShader()
{

}

bool StaticShader::InitStandard(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename)
{
	Shader::InitStandard(device, vsFilename, psFilename);

	D3D11_BUFFER_DESC	waterBufferDesc;

	waterBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	waterBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	waterBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	waterBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	waterBufferDesc.MiscFlags = 0;
	waterBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&waterBufferDesc, NULL, &m_waterBuffer);

	return false;
}

bool StaticShader::SetShaderParameters(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* world, 
	DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, Light* sceneLight1, 
	ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* textureNormals,
	DirectX::SimpleMath::Vector3 cameraPos)
{
	Shader::SetShaderParameters(context, world, view, projection, sceneLight1, texture1, textureNormals, cameraPos);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	WaterBufferType* waterPtr;

	context->Map(m_waterBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	waterPtr = (WaterBufferType*)mappedResource.pData;
	waterPtr->time = getSecondsSinceCreation();
	waterPtr->amplitude = 0.01;
	waterPtr->frequency = 0.45;

	context->Unmap(m_waterBuffer, 0);
	context->VSSetConstantBuffers(3, 1, &m_waterBuffer);

	return false;
}
