#include "pch.h"
#include "Entity.h"
#include "modelclass.h"

Entity::Entity(ID3D11Device* device, std::string modelName, std::string shaderName, Light* light, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection) {
    m_projection = projection;
    m_world = world;
    m_view = view;
    m_Light = light;

    std::string name = "Assets/Models/" + modelName + ".obj";
    char* charBuffer = new char[name.size() + 1];
    strcpy(charBuffer, name.c_str());
    model.InitializeModel(device, charBuffer);
    delete[] charBuffer;
    if (modelName[0] == 'm' && modelName[1] == 'a' && modelName[2] == 'z' && modelName[3] == 'e')
    {
        CreateDDSTextureFromFile(device, L"Assets/Textures/mazeFloor.dds", nullptr, colorTexture.ReleaseAndGetAddressOf());
        CreateDDSTextureFromFile(device, L"Assets/Textures/mazeFloor_normal.dds", nullptr, normalTexture.ReleaseAndGetAddressOf());
    }
    else
    {
        std::string nameColorT = "Assets/Textures/" + modelName + ".dds";
        std::wstring wideString = std::wstring(nameColorT.begin(), nameColorT.end());
        const wchar_t* wcharPtr = wideString.c_str();
        CreateDDSTextureFromFile(device, wcharPtr, nullptr, colorTexture.ReleaseAndGetAddressOf());

        std::string nameNormalT = "Assets/Textures/" + modelName + "_normal.dds";
        wideString = std::wstring(nameNormalT.begin(), nameNormalT.end());
        const wchar_t* wcharPtr2 = wideString.c_str();
        CreateDDSTextureFromFile(device, wcharPtr2, nullptr, normalTexture.ReleaseAndGetAddressOf());
    }
}

Entity::~Entity() {
    // Liberar recursos aquí
}

void Entity::Initialize(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 sca, DirectX::XMFLOAT3 rot)
{
    position = pos;
    scale = sca;
    rotation = rot;
}



void Entity::Render(ID3D11DeviceContext* deviceContext)
{
    setLocation();
}

void Entity::RenderShadow(ID3D11DeviceContext* deviceContext, DirectX::SimpleMath::Matrix* positionshadowView)
{
    setLocation();
}

void Entity::setShadowMap(ID3D11ShaderResourceView* texture)
{
    shadowTexture = texture;
}

void Entity::setLocation()
{
    float toRadians = 0.017453; // PI / 180
    if (!parent)
    {
        *m_world = SimpleMath::Matrix::Identity;
        //prepare transform for second object.
        SimpleMath::Matrix newScale = SimpleMath::Matrix::CreateScale(scale.x, scale.y, scale.z);

        SimpleMath::Matrix newRotationX = SimpleMath::Matrix::CreateRotationX(rotation.x * toRadians);
        SimpleMath::Matrix newRotationY = SimpleMath::Matrix::CreateRotationY(rotation.y * toRadians);
        SimpleMath::Matrix newRotationZ = SimpleMath::Matrix::CreateRotationZ(rotation.z * toRadians);

        SimpleMath::Matrix newRotation = newRotationX * newRotationY * newRotationZ;

        SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(position.x, position.y, position.z);
        *m_world = (*m_world) * newScale * newRotation * newPosition;
    }
    else
    {
        *m_world = SimpleMath::Matrix::Identity;
        //prepare transform for second object.
        SimpleMath::Matrix newScale = SimpleMath::Matrix::CreateScale(scale.x, scale.y, scale.z);

        SimpleMath::Matrix newRotationX = SimpleMath::Matrix::CreateRotationX(rotation.x * toRadians);
        SimpleMath::Matrix newRotationY = SimpleMath::Matrix::CreateRotationY(rotation.y * toRadians);
        SimpleMath::Matrix newRotationZ = SimpleMath::Matrix::CreateRotationZ(rotation.z * toRadians);

        SimpleMath::Matrix newRotation = newRotationX * newRotationY * newRotationZ;

        SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(position.x, position.y, position.z);

        SimpleMath::Matrix newRotationYParent = SimpleMath::Matrix::CreateRotationY(parentRotation.y * toRadians);
        *m_world = (*m_world) * newScale * newRotation * newPosition * newRotationYParent;
    }
}


DirectX::XMFLOAT3 Entity::getPosition()
{
    return position;
}

DirectX::XMFLOAT3 Entity::getScale()
{
    return scale;
}

DirectX::XMFLOAT3 Entity::getRotation()
{
    return rotation;
}

void Entity::setPosition(DirectX::XMFLOAT3 pos)
{
    position = pos;
}

void Entity::setRotation(DirectX::XMFLOAT3 rot)
{
    rotation = rot;
}

void Entity::setParentRotation(DirectX::XMFLOAT3 rot)
{
    parent = true;
    parentRotation = rot;
}

DirectX::XMFLOAT4 Entity::Mul(DirectX::XMFLOAT4 term1, DirectX::SimpleMath::Matrix* matrix)
{
    DirectX::XMFLOAT4 result;

    result.x = term1.x * matrix->_11 + term1.y * matrix->_21 + term1.z * matrix->_31 + term1.w * matrix->_41;
    result.y = term1.x * matrix->_12 + term1.y * matrix->_22 + term1.z * matrix->_32 + term1.w * matrix->_42;
    result.z = term1.x * matrix->_13 + term1.y * matrix->_23 + term1.z * matrix->_33 + term1.w * matrix->_43;
    result.w = term1.x * matrix->_14 + term1.y * matrix->_24 + term1.z * matrix->_34 + term1.w * matrix->_44;

    return result;
}