#include "pch.h"
#include "StaticEntity.h"
#include "modelclass.h"
#include "Shader.h"
#include <Windows.h>
#include "Light.h"
#include "Camera.h"

StaticEntity::StaticEntity(ID3D11Device* device, std::string modelName, std::string shaderName, Light* light, DirectX::SimpleMath::Matrix* world,
    DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, Camera* cam) : Entity(device, modelName, shaderName,
        light, world, view, projection) {
    camera = cam;

    std::string nameVertex = shaderName + "_vs.cso";
    std::string namePixel = shaderName + "_ps.cso";

    int size = MultiByteToWideChar(CP_UTF8, 0, nameVertex.c_str(), -1, nullptr, 0);
    WCHAR* wideNameVertex = new WCHAR[size];
    MultiByteToWideChar(CP_UTF8, 0, nameVertex.c_str(), -1, wideNameVertex, size);

    // Convertir std::string a WCHAR* para namePixel
    size = MultiByteToWideChar(CP_UTF8, 0, namePixel.c_str(), -1, nullptr, 0);
    WCHAR* wideNamePixel = new WCHAR[size];
    MultiByteToWideChar(CP_UTF8, 0, namePixel.c_str(), -1, wideNamePixel, size);

    shader.InitStandard(device, wideNameVertex, wideNamePixel);
}

StaticEntity::~StaticEntity() {
    // Liberar recursos aquí

}

void StaticEntity::Initialize(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 sca, DirectX::XMFLOAT3 rot) {
    Entity::Initialize(pos, sca, rot);

}

void StaticEntity::Update(float deltaTime) {

}

void StaticEntity::Render(ID3D11DeviceContext* context) {

    Entity::Render(context);

    shader.EnableShader(context);

    shader.SetShaderParameters(context, m_world, m_view, m_projection, m_Light,
        colorTexture.Get(), normalTexture.Get(), camera->getPosition());

    shader.setShadowView(context, shadowTexture.Get(), m_world, m_view, m_projection, lightView);

    model.Render(context);
}

void StaticEntity::RenderShadow(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* positionshadowView)
{
    Entity::RenderShadow(context, positionshadowView);

    lightView = positionshadowView;

    shader.EnableShader(context);

    shader.SetShaderParameters(context, m_world, positionshadowView, m_projection, m_Light,
        colorTexture.Get(), normalTexture.Get(), camera->getPosition());

    model.Render(context);
}

