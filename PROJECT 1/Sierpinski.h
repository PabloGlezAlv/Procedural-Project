#pragma once
#include <vector>

#define ICOUNT 2047 //MAX SIZE
#define VCOUNT 2047//MAX SIZE

class Sierpinski
{
public:
	Sierpinski() {};
	Sierpinski(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::SimpleMath::Vector3 color,
		DirectX::SimpleMath::Vector3 corner1, DirectX::SimpleMath::Vector3 corner2, DirectX::SimpleMath::Vector3 corner3,
		bool directionColor, DirectX::SimpleMath::Vector3* playerPosition);
	~Sierpinski();
	void Update(float deltaTime);

	void Render();

private:
	DirectX::PrimitiveBatch<DirectX::VertexPositionColor>*  m_batch;

	std::vector<DirectX::VertexPositionColor> points;
	DirectX::SimpleMath::Vector3 color;

	uint16_t iArray[ICOUNT];
	DirectX::VertexPositionColor vArray[VCOUNT];


	float maxY = 0;
	float minY = 99999;

	float movement = 0;
	bool colorDir;

	DirectX::SimpleMath::Vector3* playerPosition;

	float startFog = 5;
	float endFog = 20;
};

