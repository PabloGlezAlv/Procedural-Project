#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "modelclass.h"
#include "Entity.h"
#include "StaticShader.h"

class Camera;

class Boat : public Entity {
public:
    Boat() {}
    Boat(ID3D11Device* device, std::string model, std::string shaderName, Light* light, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view, 
        DirectX::SimpleMath::Matrix* projection, Camera* cam);
    ~Boat();

    void Initialize(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation);
    void Update(float deltaTime);
    void Render(ID3D11DeviceContext* deviceContext);
    void RenderShadow(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* positionshadowView);
private:

    Camera* camera;

    StaticShader shader;
};

