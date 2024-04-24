#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "modelclass.h"
#include "Entity.h"
#include "StaticShader.h"
#include "Crystal.h"

class Camera;
class Light;

class LightHouse : public Entity {
public:
    LightHouse() {}
    LightHouse(ID3D11Device* device, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, std::string model, std::string shaderName, Light* light, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view,
        DirectX::SimpleMath::Matrix* projection, Camera* cam, float rotSpeed);
    ~LightHouse();

    void Initialize(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation);
    void Update(float deltaTime);
    void Render(ID3D11DeviceContext* deviceContext);

    void RenderCrystal();

    void RenderShadow(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* positionshadowView); 

    DirectX::XMFLOAT3 getCrystalPosition();

    Light* getLight();
private:

    Camera* camera;

    Light* light;

    StaticShader shader;

    //Top rotation
    ModelClass modelTop;
    DirectX::XMFLOAT3 positionTop;
    DirectX::XMFLOAT3 scaleTop;
    DirectX::XMFLOAT3 rotationTop;

    float rotationSpeed = 0;

    //Crystal
    Crystal crystal;
};


