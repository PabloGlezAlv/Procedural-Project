#pragma once
#include <vector>
#include "Sierpinski.h"

class Crystal
{
public:
	Crystal() {};
	Crystal(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::SimpleMath::Vector3* playerPos);

	void Initialize(DirectX::SimpleMath::Vector3 color,DirectX::SimpleMath::Vector3 translate, DirectX::SimpleMath::Vector3 scale);
	~Crystal();
	void Update(float deltaTime);

	void Render();

	DirectX::XMFLOAT3 getPosition();
private:
	std::vector<Sierpinski> triangles;

	DirectX::SimpleMath::Vector3 top;
	DirectX::SimpleMath::Vector3 bottom;
	std::vector<DirectX::SimpleMath::Vector3> middle;

	DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* m_batch;

	DirectX::SimpleMath::Vector3* playerPos;

	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 color;

	float levitate = 0;

	float topY;
	float bottomY;
	float midY;
};

