#include "pch.h"
#include "Crystal.h"

Crystal::Crystal(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::SimpleMath::Vector3* playerPosition)
{
    m_batch = batch;

    playerPos = playerPosition;
}

void Crystal::Initialize(DirectX::SimpleMath::Vector3 col, DirectX::SimpleMath::Vector3 translate, DirectX::SimpleMath::Vector3 scale)
{
    position = translate;
    color = col;
    //Triangle base creation
    top = DirectX::SimpleMath::Vector3(translate.x, (0.7f * scale.y) + translate.y, translate.z);
    bottom = DirectX::SimpleMath::Vector3(translate.x, (-0.7f * scale.y) + translate.y, translate.z);

    middle.push_back(DirectX::SimpleMath::Vector3((-0.5f * scale.x) + translate.x, translate.y, (0.5f * scale.z) + translate.z));
    middle.push_back(DirectX::SimpleMath::Vector3((0.5f * scale.x) + translate.x, translate.y, (0.5f * scale.z) + translate.z));
    middle.push_back(DirectX::SimpleMath::Vector3((0.5f * scale.x) + translate.x, translate.y, (-0.5f * scale.z) + translate.z));
    middle.push_back(DirectX::SimpleMath::Vector3((-0.5f * scale.x) + translate.x, translate.y, (-0.5f * scale.z) + translate.z));

    topY = top.y;
    bottomY = bottom.y;
    midY = middle[0].y;

    //Top crystal
    for (int i = 0; i < 4; i++)
    {
        triangles.push_back(Sierpinski(m_batch, color, top, middle[i], middle[(i + 1) % 4], true, playerPos)); //Triangle with top and 2 in middle
    }

    //Bottom crystal
    for (int i = 0; i < 4; i++)
    {
        triangles.push_back(Sierpinski(m_batch, color, bottom, middle[i], middle[(i + 1) % 4], false, playerPos));
    }
}

Crystal::~Crystal()
{
    triangles.clear();
}

void Crystal::Update(float deltaTime)
{
    //Rotate levitate triangles
    levitate += deltaTime;
    if (levitate > 360)
    {
        levitate -= 360;
    }

    for (int i = 0; i < middle.size(); i++)
    {
        DirectX::SimpleMath::Vector3 point = middle[i];
        DirectX::SimpleMath::Vector3 center = position;

        DirectX::SimpleMath::Vector3 offset = point - center;

        float angle = deltaTime;

        DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(angle);

        offset = DirectX::SimpleMath::Vector3::Transform(offset, rotationMatrix);

        point = center + offset;
         
        point.y = midY + cos(levitate) / 4;

        middle[i] = point;
    }

    top.y = topY + cos(levitate) / 6;
    bottom.y = bottomY + cos(levitate) / 6;


    triangles.clear();

    //Top crystal
    for (int i = 0; i < 4; i++)
    {
        triangles.push_back(Sierpinski(m_batch, color, top, middle[i], middle[(i + 1) % 4], true, playerPos)); //Triangle with top and 2 in middle
    }

    //Bottom crystal
    for (int i = 0; i < 4; i++)
    {
        triangles.push_back(Sierpinski(m_batch, color, bottom, middle[i], middle[(i + 1) % 4], false, playerPos));
    }


    for (Sierpinski currentTriangle : triangles) {
        currentTriangle.Update(deltaTime);
    }
}

void Crystal::Render()
{
    for (Sierpinski currentTriangle : triangles) {
        currentTriangle.Render();
    }
}

DirectX::XMFLOAT3 Crystal::getPosition()
{
    return DirectX::XMFLOAT3(top.x, middle[0].y, top.z);
}
