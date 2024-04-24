#include "pch.h"
#include "Boat.h"
#include "modelclass.h"
#include "Shader.h"
#include <Windows.h>
#include "Light.h"
#include "Camera.h"

Boat::Boat(ID3D11Device* device, std::string modelName, std::string shaderName, Light* light, DirectX::SimpleMath::Matrix* world,
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

Boat::~Boat() {
    // Liberar recursos aquí
    
}

void Boat::Initialize(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 sca, DirectX::XMFLOAT3 rot) {
    Entity::Initialize(pos, sca, rot);
}


void Boat::Update(float deltaTime) {
    float s_X = 0;
    float s_Y = 0;
    float s_Z = 0;

    DirectX::XMFLOAT2 X0 = DirectX::XMFLOAT2(position.x, position.z);
    float height = 0;

    float time = shader.getSecondsSinceCreation();

    for (int i = 0; i < 3; i++)
    {
        DirectX::XMFLOAT2 K = { 1, static_cast<float>(i) };
        float a = 0.01; // a is amplitude

        DirectX::XMFLOAT2 X = { X0.x - K.x * a * (float)std::sin((K.x * X0.x + K.y * X0.y) - 0.45 * time * 10),
                    X0.y - K.y * a * (float)std::sin((K.x * X0.x + K.y * X0.y) - 0.45 * time * 10) };

        float y = a * std::cos((K.x * X0.x + K.y * X0.y) - 0.45 * time * 10);

        s_X += X.x / 64 - 0.5f;
        s_Y += X.y / 64 - 0.5f;
        s_Z += y;
    }
    DirectX::XMFLOAT4 finalPos;
    finalPos.x = s_X * 7.5f;
    finalPos.z = s_Y * 7.5f;
    finalPos.y = s_Z * 7.5f;
    finalPos.w = 1;

    //finalPos = Mul(finalPos, m_world);

    float lastPositionY = position.y;
    float error = 0 ;
    position.y = finalPos.y + finalPos.y * error;

    float dir = (lastPositionY - position.y);

    rotation.x += dir * 30;
}

void Boat::Render(ID3D11DeviceContext* context) {

    Entity::Render(context);

    shader.EnableShader(context);

    shader.SetShaderParameters(context, m_world, m_view, m_projection, m_Light, 
        colorTexture.Get(), normalTexture.Get(), camera->getPosition());

    shader.setShadowView(context, shadowTexture.Get(), m_world, m_view, m_projection, lightView);

    model.Render(context);
}

void Boat::RenderShadow(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* positionshadowView)
{
    Entity::RenderShadow(context, positionshadowView);

    lightView = positionshadowView;

    shader.EnableShader(context);

    shader.SetShaderParameters(context, m_world, positionshadowView, m_projection, m_Light,
        colorTexture.Get(), normalTexture.Get(), camera->getPosition());

    model.Render(context);
}

