#include "pch.h"
#include "Sierpinski.h"

Sierpinski::Sierpinski(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::SimpleMath::Vector3 col,
    DirectX::SimpleMath::Vector3 corner1, DirectX::SimpleMath::Vector3 corner2, DirectX::SimpleMath::Vector3 corner3, 
    bool direction, DirectX::SimpleMath::Vector3* playerPos)
{
    m_batch = batch;
    color = col;
    playerPosition = playerPos;
    colorDir = direction;

    DirectX::XMVECTORF32 colorf = { {{(col.x), (col.y), (col.z), (1)}} };
    DirectX::XMVECTORF32 colorf2 = { {{(col.x), (col.y), (col.z), (0.1)}} };
    DirectX::XMVECTORF32 colorf3 = { {{(col.x), (col.y), (col.z), (0.1)}} };

    points.push_back(DirectX::VertexPositionColor(corner1, colorf));
    points.push_back(DirectX::VertexPositionColor(corner2, colorf2));
    points.push_back(DirectX::VertexPositionColor(corner3, colorf3));


    iArray[0] = 0;
    iArray[1] = 1;
    iArray[2] = 2;

    vArray[0] = points[0];
    vArray[1] = points[1];
    vArray[2] = points[2];

    for (int i = 3; i < ICOUNT; i++)
    {
        DirectX::VertexPositionColor aux = vArray[rand() % 3];

        DirectX::SimpleMath::Vector3 vPos = DirectX::SimpleMath::Vector3(aux.position.x + vArray[i - 1].position.x, aux.position.y + vArray[i - 1].position.y, aux.position.z + vArray[i - 1].position.z);

        DirectX::XMVECTORF32 colorf = { {{(col.x), (col.y), (col.z), 1}} };

        if (minY > vPos.y)
            minY = vPos.y;
        if (maxY < vPos.y)
            maxY = vPos.y;

        DirectX::VertexPositionColor point = DirectX::VertexPositionColor(vPos / 2.0, colorf);
        iArray[i] = i;
        vArray[i] = point;
    }
}

Sierpinski::~Sierpinski()
{
}

void Sierpinski::Update(float deltaTime)
{
    movement += deltaTime;

    if (((maxY + minY) / 2) < movement)
    {
        movement -= ((maxY + minY) / 2);
    }
}

void Sierpinski::Render()
{
    m_batch->Begin();


    float half = ((maxY + minY) / 2) + movement;
    for (int i = 3; i < ICOUNT; i++)
    {
        DirectX::VertexPositionColor aux = vArray[rand() % 3];

        DirectX::SimpleMath::Vector3 vPos = DirectX::SimpleMath::Vector3(aux.position.x + vArray[i - 1].position.x, aux.position.y + vArray[i - 1].position.y, aux.position.z + vArray[i - 1].position.z);;

        float yellow = (vPos.y - minY) / (maxY - minY);

        if (colorDir)
            yellow = 1 - yellow;

        DirectX::XMVECTORF32 colorf = { {{(yellow * color.x), (yellow * color.y), (color.z), 0.8}} };

        DirectX::VertexPositionColor point = DirectX::VertexPositionColor(vPos / 2.0, colorf);
        iArray[i] = i;
        vArray[i] = point;
    }

    //Draw
    m_batch->DrawIndexed(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST, iArray, ICOUNT, vArray, VCOUNT);

    m_batch->End();
}
