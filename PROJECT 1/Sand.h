#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Entity.h"
#include "SandShader.h"

class Camera;

class Sand : public Entity {
public:
    Sand() {}
    Sand(ID3D11Device* device, std::string model, std::string shaderName, Light* light, DirectX::SimpleMath::Matrix* world, 
        DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, Camera* cam);
    ~Sand();

    void Initialize(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rot);
    void Update(float deltaTime);
    void Render(ID3D11DeviceContext* deviceContext);
    void RenderShadow(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* positionshadowView);

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> heightTexture;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> wetSandTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> wetSandNormalTexture;

    Camera* camera;

    SandShader shader;
};


