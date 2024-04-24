#include "pch.h"
#include "MarchingTerrain.h"
#include <SimpleMath.h>

MarchingTerrain::MarchingTerrain(ID3D11Device* device, Shader* basicShaderPair, Light* light, ID3D11ShaderResourceView* m_texture,
	DirectX::PrimitiveBatch<DirectX::VertexPositionNormalTexture>*  batch, DirectX::SimpleMath::Matrix* m_world, DirectX::SimpleMath::Matrix* m_view,
	DirectX::SimpleMath::Matrix* m_projection, double isolevel, int width, int depth, int height)
{
	m_terrainWidth = width;
	m_terrainHeight = height;
	m_terrainDepth = depth;
	isoLevel = isolevel;

	m_BasicShaderPair = basicShaderPair;
	m_Light = light;
	texture = m_texture;

	m_batch = batch;

	world = m_world;
	view = m_view;
	projection = m_projection;
	CreateSphere();

	//test
	triangles = new TRIANGLE[5];
	GenerateMarchingCubes(device, isolevel);
}

void MarchingTerrain::GenerateMarchingCubes(ID3D11Device* device, double isolevel)
{
	GRIDCELL cell;
	cell.myPoints[0] = DirectX::XMFLOAT3(-5.0f, -5.0f, 5.0f);    // Back bottom left
	cell.myPoints[1] = DirectX::XMFLOAT3(5.0f, -5.0f, 5.0f);     // Back bottom right
	cell.myPoints[2] = DirectX::XMFLOAT3(5.0f, -5.0f, -5.0f);    // Front bottom right
	cell.myPoints[3] = DirectX::XMFLOAT3(-5.0f, -5.0f, -5.0f);   // Front bottom left
	cell.myPoints[4] = DirectX::XMFLOAT3(-5.0f, 5.0f, 5.0f);     // Back top left
	cell.myPoints[5] = DirectX::XMFLOAT3(5.0f, 5.0f, 5.0f);      // Back top right
	cell.myPoints[6] = DirectX::XMFLOAT3(5.0f, 5.0f, -5.0f);     // Front top right
	cell.myPoints[7] = DirectX::XMFLOAT3(-5.0f, 5.0f, -5.0f);    // Front top left

	int i = 0;
	cell.val[i++] = 0;
	cell.val[i++] = 0;
	cell.val[i++] = 0;
	cell.val[i++] = 0;
	cell.val[i++] = 1;
	cell.val[i++] = 1;
	cell.val[i++] = 1;
	cell.val[i++] = 1;


	numTriangles = marchingCubes.Polygonise(cell, isolevel, triangles);
}

DirectX::SimpleMath::Vector3 getNormals(DirectX::SimpleMath::Vector3 vertex1, DirectX::SimpleMath::Vector3 vertex2, DirectX::SimpleMath::Vector3 vertex3)
{
	DirectX::SimpleMath::Vector3 normals, vector1, vector2;
	
	vector1 = vertex2 - vertex1;
	vector2 = vertex3 - vertex1;

	vector1.DirectX::SimpleMath::Vector3::Vector3::Cross(vector2, normals);
	normals.DirectX::SimpleMath::Vector3::Vector3::Normalize();

	return normals;
}

void MarchingTerrain::Render(ID3D11DeviceContext* context)
{
	m_batch->Begin();
	// Assuming m_BasicShaderPair is capable of rendering triangles
	m_BasicShaderPair->EnableShader(context);
	m_BasicShaderPair->SetShaderParameters(context, world, view, projection, m_Light, texture);

	RenderTerrain();

	m_batch->End();
}

void MarchingTerrain::RenderTerrain()
{
	const int vCount = 3;
	const int iCount = 3;

	for (int x = 0; x < m_terrainWidth; ++x) {
		for (int y = 0; y < m_terrainHeight; ++y) {
			for (int z = 0; z < m_terrainDepth; ++z) {
				//Access each block
				numTriangles = marchingCubes.Polygonise(grid[x][y][z], isoLevel, triangles);

				//Draw the triangles of each block
				for (size_t i = 0; i < numTriangles; i++)
				{
					uint16_t iArray[iCount];
					DirectX::VertexPositionNormalTexture vArray[vCount];
					iArray[0] = 0;
					iArray[1] = 2;
					iArray[2] = 1;

					DirectX::SimpleMath::Vector3 position0 = DirectX::SimpleMath::Vector3::Vector3(triangles[i].p[0].x, triangles[i].p[0].y, triangles[i].p[0].z);
					DirectX::SimpleMath::Vector3 position1 = DirectX::SimpleMath::Vector3::Vector3(triangles[i].p[1].x, triangles[i].p[1].y, triangles[i].p[1].z);
					DirectX::SimpleMath::Vector3 position2 = DirectX::SimpleMath::Vector3::Vector3(triangles[i].p[2].x, triangles[i].p[2].y, triangles[i].p[2].z);

					if (position0 != position1 && position0 != position2 && position1 != position2)
					{
						DirectX::SimpleMath::Vector3 normals = getNormals(position0, position2, position1);
						if (normals.y == 1.f)
						{
							int i = 0;
						}

						//normals = DirectX::SimpleMath::Vector3::Vector3(0,1,0);

						vArray[0] = DirectX::VertexPositionNormalTexture(position0, normals,
							DirectX::SimpleMath::Vector2::Vector2(0, 0));
						vArray[1] = DirectX::VertexPositionNormalTexture(position1, normals,
							DirectX::SimpleMath::Vector2::Vector2(0, 0));
						vArray[2] = DirectX::VertexPositionNormalTexture(position2, normals,
							DirectX::SimpleMath::Vector2::Vector2(0, 0));


						//Draw
						m_batch->DrawIndexed(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, iArray, iCount, vArray, vCount);
						// 
						//m_batch->DrawTriangle(vArray[0], vArray[1], vArray[2]);
					}


				}
			}
		}
	}
}

void MarchingTerrain::CreateTerrain()
{
	grid.resize(m_terrainWidth, std::vector<std::vector<GRIDCELL>>(m_terrainHeight, std::vector<GRIDCELL>(m_terrainDepth)));

	DirectX::XMFLOAT3 center = DirectX::XMFLOAT3(m_terrainWidth / 2, m_terrainHeight / 2, m_terrainDepth / 2);
	float radio = m_terrainHeight / 2;

	for (int x = 0; x < m_terrainWidth; ++x) {
		for (int y = 0; y < m_terrainHeight; ++y) {
			for (int z = 0; z < m_terrainDepth; ++z) {
				GRIDCELL& cell = grid[x][y][z];

				// Set value for the box
				cell.myPoints[0] = DirectX::XMFLOAT3(x, y, z);
				cell.myPoints[1] = DirectX::XMFLOAT3(x + 1, y, z);
				cell.myPoints[2] = DirectX::XMFLOAT3(x + 1, y, z + 1);
				cell.myPoints[3] = DirectX::XMFLOAT3(x, y, z + 1);
				cell.myPoints[4] = DirectX::XMFLOAT3(x, y + 1, z);
				cell.myPoints[5] = DirectX::XMFLOAT3(x + 1, y + 1, z);
				cell.myPoints[6] = DirectX::XMFLOAT3(x + 1, y + 1, z + 1);
				cell.myPoints[7] = DirectX::XMFLOAT3(x, y + 1, z + 1);

				//// For the moment only the top will have to draw
				for (int i = 0; i < 8; ++i) {
					if (i > 3 && (y == m_terrainHeight/2 || y == m_terrainHeight / 2 + 1))
						cell.val[i] = 0;	
					else
						cell.val[i] = 1;
				}

			}
		}
	}
}

void MarchingTerrain::CreateSphere()
{
	grid.resize(m_terrainWidth, std::vector<std::vector<GRIDCELL>>(m_terrainHeight, std::vector<GRIDCELL>(m_terrainDepth)));

	DirectX::XMFLOAT3 center = DirectX::XMFLOAT3(m_terrainWidth / 2, m_terrainHeight / 2, m_terrainDepth / 2);
	float radio = m_terrainHeight / 3;



	for (int x = 0; x < m_terrainWidth; ++x) {
		for (int y = 0; y < m_terrainHeight; ++y) {
			for (int z = 0; z < m_terrainDepth; ++z) {
				GRIDCELL& cell = grid[x][y][z];

				// Set value for the box
				cell.myPoints[0] = DirectX::XMFLOAT3(x, y, z);
				cell.myPoints[1] = DirectX::XMFLOAT3(x + 1, y, z);
				cell.myPoints[2] = DirectX::XMFLOAT3(x + 1, y, z + 1);
				cell.myPoints[3] = DirectX::XMFLOAT3(x, y, z + 1);
				cell.myPoints[4] = DirectX::XMFLOAT3(x, y + 1, z);
				cell.myPoints[5] = DirectX::XMFLOAT3(x + 1, y + 1, z);
				cell.myPoints[6] = DirectX::XMFLOAT3(x + 1, y + 1, z + 1);
				cell.myPoints[7] = DirectX::XMFLOAT3(x, y + 1, z + 1);

				//// For the moment only the top will have to draw
				for (int i = 0; i < 8; ++i) {
					float distance = sqrt(pow(cell.myPoints[i].x - center.x, 2) + pow(cell.myPoints[i].y - center.y, 2) + pow(cell.myPoints[i].z - center.z, 2));

					if (distance <= radio)
					{
						if(radio - distance < 1)
							cell.val[i] = radio - distance;
						else cell.val[i] = 1;
					}
					else
						cell.val[i] = 0;
				}

			}
		}
	}
}

MarchingTerrain::~MarchingTerrain()
{

}
