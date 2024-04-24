#pragma once
#include "MarchingCubes.h"
#include "Shader.h"


class MarchingTerrain
{
public:
	MarchingTerrain() {}
	~MarchingTerrain();
	MarchingTerrain(ID3D11Device* device, Shader* m_BasicShaderPair, Light* m_Light, ID3D11ShaderResourceView* m_texture,
		DirectX::PrimitiveBatch<DirectX::VertexPositionNormalTexture>* batch, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view,
		DirectX::SimpleMath::Matrix* projection,
		double isolevel, int width, int depth, int height );

	void Render(ID3D11DeviceContext* deviceContext);


	//Test
	void GenerateMarchingCubes(ID3D11Device* device, double isolevel);
private:
	MarchingCubes marchingCubes;

	DirectX::PrimitiveBatch<DirectX::VertexPositionNormalTexture>*  m_batch;
	//Position
	DirectX::SimpleMath::Matrix* world;
	DirectX::SimpleMath::Matrix* view;
	DirectX::SimpleMath::Matrix* projection;
	//Parameters
	int m_terrainWidth, m_terrainHeight, m_terrainDepth;
	double isoLevel;
	std::vector<std::vector<std::vector<GRIDCELL>>> grid;

	//Texturing
	Shader* m_BasicShaderPair;
	Light* m_Light;
	ID3D11ShaderResourceView* texture;

	//Test
	TRIANGLE* triangles;
	int numTriangles;
	//----------------------------------------------------------
	void CreateSphere();
	void CreateTerrain();

	void RenderTerrain();
};

