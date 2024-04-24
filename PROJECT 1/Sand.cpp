#include "pch.h"
#include "Sand.h"
#include "modelclass.h"
#include "Camera.h"

Sand::Sand(ID3D11Device* device, std::string modelName, std::string shaderName, Light* light, DirectX::SimpleMath::Matrix* world,
    DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, Camera* cam) : Entity(device, modelName, shaderName,
        light, world, view, projection) {
    std::string nameColorT = "Assets/Textures/" + modelName + "_height.dds";
    std::wstring wideString = std::wstring(nameColorT.begin(), nameColorT.end());
    const wchar_t* wcharPtr = wideString.c_str();
    CreateDDSTextureFromFile(device, wcharPtr, nullptr, heightTexture.ReleaseAndGetAddressOf());

    nameColorT = "Assets/Textures/" + modelName + "Wet.dds";
    wideString = std::wstring(nameColorT.begin(), nameColorT.end());
    const wchar_t* wcharPtr2 = wideString.c_str();
    CreateDDSTextureFromFile(device, wcharPtr2, nullptr, wetSandTexture.ReleaseAndGetAddressOf());

    nameColorT = "Assets/Textures/" + modelName + "Wet_normal.dds";
    wideString = std::wstring(nameColorT.begin(), nameColorT.end());
    const wchar_t* wcharPtr3 = wideString.c_str();
    CreateDDSTextureFromFile(device, wcharPtr3, nullptr, wetSandNormalTexture.ReleaseAndGetAddressOf());

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

Sand::~Sand() {
    // Liberar recursos aquí

}

void Sand::Initialize(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 sca, DirectX::XMFLOAT3 rot) {
    Entity::Initialize(pos, sca, rot);


}

void Sand::Update(float deltaTime) {

}

void Sand::Render(ID3D11DeviceContext* context) {
    Entity::Render(context);

    shader.EnableShader(context);

    shader.SetShaderParameters(context, m_world, m_view, m_projection, m_Light, colorTexture.Get(), 
        normalTexture.Get(),camera->getPosition(), heightTexture.Get(), wetSandTexture.Get(), wetSandNormalTexture.Get());

    shader.setShadowView(context, shadowTexture.Get(), m_world, m_view, m_projection, lightView);

    model.Render(context);
}

void Sand::RenderShadow(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* positionshadowView)
{
    Entity::RenderShadow(context, positionshadowView);

    lightView = positionshadowView;

    shader.EnableShader(context);

    shader.SetShaderParameters(context, m_world, positionshadowView, m_projection, m_Light, colorTexture.Get(),
        normalTexture.Get(), camera->getPosition(), heightTexture.Get(), wetSandTexture.Get(), wetSandNormalTexture.Get());
    model.Render(context);
}