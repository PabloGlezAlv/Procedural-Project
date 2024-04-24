#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "modelclass.h"
#include "Entity.h"
#include "StaticShader.h"

class Camera;

class StaticEntity : public Entity {
public:
    StaticEntity() {}
    StaticEntity(ID3D11Device* device, std::string model, std::string shaderName, Light* light, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view,
        DirectX::SimpleMath::Matrix* projection, Camera* cam);
    ~StaticEntity();

    void Initialize(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rot);
    void Update(float deltaTime);
    void Render(ID3D11DeviceContext* deviceContext);
    void RenderShadow(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* positionshadowView);
private:
    Camera* camera;

    StaticShader shader;
};

