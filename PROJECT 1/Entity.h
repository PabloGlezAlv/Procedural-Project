#pragma once
#include "modelclass.h"
#include "Light.h"

class Entity
{
public:
    Entity() {}
    Entity(ID3D11Device* device, std::string model, std::string shaderName, Light* light, 
        DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view, 
        DirectX::SimpleMath::Matrix* projection);
    ~Entity();

    virtual void Initialize(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation);
    virtual void Update(float deltaTime) = 0;
    virtual void Render(ID3D11DeviceContext* deviceContext);
    virtual void RenderShadow(ID3D11DeviceContext* deviceContext, DirectX::SimpleMath::Matrix* positionshadowView);

    void setShadowMap(ID3D11ShaderResourceView* texture);

    DirectX::XMFLOAT3 getPosition();
    DirectX::XMFLOAT3 getScale();
    DirectX::XMFLOAT3 getRotation();

    void setPosition(DirectX::XMFLOAT3 pos);
    void setRotation(DirectX::XMFLOAT3 rot);
    void setParentRotation(DirectX::XMFLOAT3 rot);
protected:
    DirectX::XMFLOAT4 Mul(DirectX::XMFLOAT4 term1, DirectX::SimpleMath::Matrix* matrix);

    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 scale;
    DirectX::XMFLOAT3 rotation;

    bool parent = false;
    DirectX::XMFLOAT3 parentRotation;

    ModelClass model;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        colorTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        normalTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        shadowTexture;

    DirectX::SimpleMath::Matrix* m_world;
    DirectX::SimpleMath::Matrix* m_view;
    DirectX::SimpleMath::Matrix* m_projection;

    DirectX::SimpleMath::Matrix* lightView;

    Light* m_Light;

private:
    void setLocation();

};

