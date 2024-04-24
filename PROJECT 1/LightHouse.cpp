#include "pch.h"
#include "LightHouse.h"
#include "Shader.h"
#include <Windows.h>
#include "Light.h"
#include "Camera.h"

LightHouse::LightHouse(ID3D11Device* device, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, std::string modelName, std::string shaderName, Light* light, DirectX::SimpleMath::Matrix* world,
    DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, Camera* cam, float rotSpeed) : Entity(device, modelName, shaderName,
        light, world, view, projection) {
    camera = cam;
    rotationSpeed = rotSpeed;

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

    //Top rotation
    std::string name = "Assets/Models/" + modelName + "Top.obj";
    char* charBuffer = new char[name.size() + 1];
    strcpy(charBuffer, name.c_str());
    modelTop.InitializeModel(device, charBuffer);
    delete[] charBuffer;

    //Top crystal 

    crystal = Crystal(batch, &cam->getPosition());
}

LightHouse::~LightHouse() {
    // Liberar recursos aquí

}

void LightHouse::Initialize(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 sca, DirectX::XMFLOAT3 rot) {
    Entity::Initialize(pos, sca, rot);

    positionTop = pos;
    scaleTop = sca;
    rotationTop = rot;

    DirectX::SimpleMath::Vector3 crystalPosition = pos;
    crystalPosition.y += 8.15f * sca.y;

    crystal.Initialize(DirectX::SimpleMath::Vector3(1.f, 1.f, 0.f), crystalPosition, DirectX::SimpleMath::Vector3(sca.x * 0.65, sca.y * 0.65, sca.z * 0.65));
}

void LightHouse::Update(float deltaTime) {
    
    // Update the rotation of the top lighthouse
    float rot = rotationSpeed *1.1 * deltaTime;

    rotationTop.y -= rot;

    if (rotationTop.y <= 360.0f)
    {
        rotationTop.y += 360.0f;
    }


    crystal.Update(deltaTime);
}

void LightHouse::Render(ID3D11DeviceContext* context) {

    Entity::Render(context);

    shader.EnableShader(context);

    shader.SetShaderParameters(context, m_world, m_view, m_projection, m_Light,
        colorTexture.Get(), normalTexture.Get(), camera->getPosition());

   // shader.setShadowView(context, shadowTexture.Get(), m_world, m_view, m_projection, lightView);

    model.Render(context);

    //-------------Rotate Top-------------------
    *m_world = SimpleMath::Matrix::Identity;
    //prepare transform for second object.
    SimpleMath::Matrix newScale = SimpleMath::Matrix::CreateScale(scaleTop.x, scaleTop.y, scaleTop.z);

    float toRadians = 0.017453; // PI / 180
    SimpleMath::Matrix newRotationX = SimpleMath::Matrix::CreateRotationX(rotationTop.x * toRadians);
    SimpleMath::Matrix newRotationY = SimpleMath::Matrix::CreateRotationY(rotationTop.y * toRadians);
    SimpleMath::Matrix newRotationZ = SimpleMath::Matrix::CreateRotationZ(rotationTop.z * toRadians);

    SimpleMath::Matrix newRotation = newRotationX * newRotationY * newRotationZ;

    SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(positionTop.x, positionTop.y, positionTop.z);
    *m_world = (*m_world) * newScale * newRotation * newPosition;

    shader.SetShaderParameters(context, m_world, m_view, m_projection, m_Light,
        colorTexture.Get(), normalTexture.Get(), camera->getPosition());

    modelTop.Render(context);
}

void LightHouse::RenderCrystal()
{
    crystal.Render();
}

void LightHouse::RenderShadow(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* positionshadowView)
{
    Entity::RenderShadow(context, positionshadowView);

    lightView = positionshadowView;

    shader.EnableShader(context);

    shader.SetShaderParameters(context, m_world, positionshadowView, m_projection, m_Light,
        colorTexture.Get(), normalTexture.Get(), camera->getPosition());

    model.Render(context);
    
    //-------------Rotate Top-------------------
    *m_world = SimpleMath::Matrix::Identity;
    //prepare transform for second object.
    SimpleMath::Matrix newScale = SimpleMath::Matrix::CreateScale(scaleTop.x, scaleTop.y, scaleTop.z);

    float toRadians = 0.017453; // PI / 180
    SimpleMath::Matrix newRotationX = SimpleMath::Matrix::CreateRotationX(rotationTop.x * toRadians);
    SimpleMath::Matrix newRotationY = SimpleMath::Matrix::CreateRotationY(rotationTop.y * toRadians);
    SimpleMath::Matrix newRotationZ = SimpleMath::Matrix::CreateRotationZ(rotationTop.z * toRadians);

    SimpleMath::Matrix newRotation = newRotationX * newRotationY * newRotationZ;

    SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(positionTop.x, positionTop.y, positionTop.z);
    *m_world = (*m_world) * newScale * newRotation * newPosition;

    shader.SetShaderParameters(context, m_world, positionshadowView, m_projection, m_Light,
        colorTexture.Get(), normalTexture.Get(), camera->getPosition());

    modelTop.Render(context);
}

DirectX::XMFLOAT3 LightHouse::getCrystalPosition()
{
    return crystal.getPosition();
}

Light* LightHouse::getLight()
{
    return light;
}

