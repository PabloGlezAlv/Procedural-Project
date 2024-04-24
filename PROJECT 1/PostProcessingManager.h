#pragma once
enum PostProcessEffect { none, dof, vignette, bw };

struct InputCommands;

class PostProcessingManager {
public:
    PostProcessingManager(){}
    PostProcessingManager(ID3D11Device* device);
    void HandleEvent(InputCommands* input);
    void Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* rView, ID3D11ShaderResourceView* rShadowView);

private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaderDOF;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaderVignette;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaderBlackWhite;

    PostProcessEffect effect = none;
};