#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "modelclass.h"
#include "Shader.h"
#include "Entity.h"
#include "WaterShader.h"

class Camera;

class Sea : public Entity{
public:
    Sea() {}
    Sea(ID3D11Device* device, std::string model, std::string shaderName, Light* light, 
        DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view, 
        DirectX::SimpleMath::Matrix* projection, Camera* cam);
    ~Sea();

    void Initialize(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rot);
    void Update(float deltaTime);
    void Render(ID3D11DeviceContext* deviceContext);
    void RenderShadow(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* positionshadowView);

private:

    WaterShader shader;

    Camera* camera;

    bool underWater = false;
};

