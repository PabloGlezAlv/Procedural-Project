#include "pch.h"
#include "SandShader.h"

SandShader::SandShader() : Shader()
{
}


SandShader::~SandShader()
{

}

bool SandShader::InitStandard(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename)
{
    Shader::InitStandard(device, vsFilename, psFilename);

    return false;
}

bool SandShader::SetShaderParameters(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* world,
    DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, Light* sceneLight1,
    ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* textureNormals, 
    DirectX::SimpleMath::Vector3 cameraPos,
    ID3D11ShaderResourceView* heightTexture, ID3D11ShaderResourceView* wetSandTexture, ID3D11ShaderResourceView* wetSandNormalTexture)
{
    Shader::SetShaderParameters(context, world, view, projection, sceneLight1, texture1, textureNormals, cameraPos);

    //------------SAND SHADERPARAMETERS--------------------
    context->VSSetSamplers(0, 1, &m_wrapState);
    context->VSSetShaderResources(0, 1, &heightTexture);
    context->PSSetShaderResources(3, 1, &wetSandTexture);
    context->PSSetShaderResources(4, 1, &wetSandNormalTexture);

    return false;
}
