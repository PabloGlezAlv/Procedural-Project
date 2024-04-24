#include "pch.h"
#include "PostProcessingManager.h"
#include "Input.h"

PostProcessingManager::PostProcessingManager(ID3D11Device* device)
{
    auto pixelShaderBuffer = DX::ReadData(L"DOF_ps.cso");
    device->CreatePixelShader(pixelShaderBuffer.data(), pixelShaderBuffer.size(), NULL, &pixelShaderDOF);

    pixelShaderBuffer = DX::ReadData(L"Vignette_ps.cso");
    device->CreatePixelShader(pixelShaderBuffer.data(), pixelShaderBuffer.size(), NULL, &pixelShaderVignette);

    pixelShaderBuffer = DX::ReadData(L"blackWhite_ps.cso");
    device->CreatePixelShader(pixelShaderBuffer.data(), pixelShaderBuffer.size(), NULL, &pixelShaderBlackWhite);
}

void PostProcessingManager::HandleEvent(InputCommands* input)
{
    if (input->R)
    {
        effect = none;
    }
    else if (input->B)
    {
        effect = bw;
    }
    else if (input->F)
    {
        effect = dof;
    }
    else if (input->V)
    {
        effect = vignette;
    }
}

void PostProcessingManager::Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* rView, ID3D11ShaderResourceView* rShadowView)
{
    switch (effect)
    {
    case dof:
        context->PSSetShader(pixelShaderDOF.Get(), nullptr, 0);
        context->PSSetShaderResources(0, 1, &rView);
        context->PSSetShaderResources(1, 1, &rShadowView);
        break;
    case vignette:
        context->PSSetShader(pixelShaderVignette.Get(), nullptr, 0);
        context->PSSetShaderResources(0, 1, &rView);
        context->PSSetShaderResources(1, 1, &rShadowView);
        break;
    case bw:
        context->PSSetShader(pixelShaderBlackWhite.Get(), nullptr, 0);
        context->PSSetShaderResources(0, 1, &rView);
        context->PSSetShaderResources(1, 1, &rShadowView);
        break;
    default:
        break;
    }
}
