#include "pch.h"
#include "Sea.h"
#include "modelclass.h"
#include "Shader.h"
#include <Windows.h>
#include "Light.h"
#include "Camera.h"

Sea::Sea(ID3D11Device* device, std::string modelName, std::string shaderName, Light* light, DirectX::SimpleMath::Matrix* world,
    DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, Camera* cam) : Entity(device, modelName, shaderName,
        light, world, view, projection) {

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

    camera = cam;

    delete[] wideNameVertex;
    delete[] wideNamePixel;
}

Sea::~Sea() {
    // Liberar recursos aquí

}

void Sea::Initialize(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 sca, DirectX::XMFLOAT3 rot) {
    Entity::Initialize(pos, sca, rot);


}

void Sea::Update(float deltaTime) {
    bool belowWater = false;

    float s_X = 0;
    float s_Y = 0;
    float s_Z = 0;

    DirectX::XMFLOAT3 cameraPos = camera->getPosition();

    DirectX::XMFLOAT2 X0 = DirectX::XMFLOAT2(cameraPos.x, cameraPos.z);
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


    float positonToCheck = finalPos.y;
    if (positonToCheck > cameraPos.y)
        belowWater = true;
    else
        belowWater = false;

    //Change fog
    if (belowWater != underWater)
    {
        underWater = belowWater;
        if(underWater)
            shader.setFogParameters(2, 12, underWater);
        else
            shader.setFogParameters(5, 20, underWater);
    }
}

void Sea::Render(ID3D11DeviceContext* context) {

    Entity::Render(context);

    shader.EnableShader(context);

    shader.SetShaderParameters(context, m_world, m_view, m_projection, m_Light, colorTexture.Get(),
        normalTexture.Get(), camera->getPosition());

   // shader.setShadowView(context, shadowTexture.Get(), m_world, m_view, m_projection, lightView);

    model.Render(context);
}

void Sea::RenderShadow(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* positionshadowView)
{
    lightView = positionshadowView;

    Entity::RenderShadow(context, positionshadowView);

    shader.EnableShader(context);

    shader.SetShaderParameters(context, m_world, positionshadowView, m_projection, m_Light, colorTexture.Get(),
        normalTexture.Get(), camera->getPosition());
    model.Render(context);
}