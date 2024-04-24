#include "pch.h"
#include "Terrain.h"
#include <cmath>


#include <iostream>
#define FNL_IMPL
#include "FastNoiseLite.h"

Terrain::Terrain(ID3D11Device* device, Light* light,
	DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view,
	DirectX::SimpleMath::Matrix* projection, Camera* cam)
{
	camera = cam;

	m_terrainGeneratedToggle = false;
	m_projection = projection;
	m_world = world;
	m_view = view;
	m_Light = light;

	CreateDDSTextureFromFile(device, L"Assets/Textures/snow.dds", nullptr, snowTexture.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Assets/Textures/snow_normal.dds", nullptr, snowNormalTexture.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Assets/Textures/sand.dds", nullptr, sandTexture.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Assets/Textures/sand_normal.dds", nullptr, sandNormalTexture.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Assets/Textures/grass.dds", nullptr, grassTexture.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Assets/Textures/grass_normal.dds", nullptr, grassNormalTexture.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Assets/Textures/rock.dds", nullptr, rockTexture.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Assets/Textures/rock_normal.dds", nullptr, rockNormalTexture.ReleaseAndGetAddressOf());


	//-----------------LOAD SHADER-----------------------------------------
	std::string nameVertex = "terrain_vs.cso";
	std::string namePixel = "terrain_ps.cso";

	int size = MultiByteToWideChar(CP_UTF8, 0, nameVertex.c_str(), -1, nullptr, 0);
	WCHAR* wideNameVertex = new WCHAR[size];
	MultiByteToWideChar(CP_UTF8, 0, nameVertex.c_str(), -1, wideNameVertex, size);

	// Convertir std::string a WCHAR* para namePixel
	size = MultiByteToWideChar(CP_UTF8, 0, namePixel.c_str(), -1, nullptr, 0);
	WCHAR* wideNamePixel = new WCHAR[size];
	MultiByteToWideChar(CP_UTF8, 0, namePixel.c_str(), -1, wideNamePixel, size);

	shader.InitStandard(device, wideNameVertex, wideNamePixel);
}


Terrain::~Terrain()
{
}

bool Terrain::Initialize(ID3D11Device* device, int terrainWidth, int terrainDepth, int terrainHeight, 
	DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 sca, DirectX::XMFLOAT3 rot)
{
	int index;
	float height = 0.0;
	bool result; 

	position = pos;
	scale = sca;
	rotation = rot;

	// Save the dimensions of the terrain.
	m_terrainWidth = terrainWidth;
	m_terrainDepth = terrainDepth;
	maxHeight = terrainHeight;
	minHeight = -terrainHeight / 2;

	m_frequency = m_terrainWidth / 20;
	m_amplitude = 3.0;
	m_wavelength = 1;

	// Create the structure to hold the terrain data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainDepth];
	if (!m_heightMap)
	{
		return false;
	}

	//this is how we calculate the texture coordinates first calculate the step size there will be between vertices. 
	float textureCoordinatesStep = 5.0f / m_terrainWidth;  //tile 5 times across the terrain. 
	// Initialise the data in the height map (flat).
	for (int j = 0; j< m_terrainDepth; j++)
	{
		for (int i = 0; i<m_terrainWidth; i++)
		{
			index = (m_terrainDepth * j) + i;

			if (i == 500 && j == 500)
			{
				int r = 0;
			}

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)j;

			//and use this step to calculate the texture coordinates for this point on the terrain.
			m_heightMap[index].u = (float)i * textureCoordinatesStep;
			m_heightMap[index].v = (float)j * textureCoordinatesStep;

		}
	}

	//even though we are generating a flat terrain, we still need to normalise it. 
	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	GenerateHeightMap(device);

	return true;
}

void Terrain::Render(ID3D11DeviceContext * deviceContext)
{
	setLocation();

	shader.EnableShader(deviceContext);

	shader.SetShaderParameters(deviceContext, m_world, m_view, m_projection, m_Light,
		sandTexture.Get(), sandNormalTexture.Get(), grassTexture.Get(), grassNormalTexture.Get(), snowTexture.Get(), snowNormalTexture.Get(), rockTexture.Get(), rockNormalTexture.Get(),
		camera->getPosition());

	shader.setShadowView(deviceContext, shadowTexture.Get(), m_world, m_view, m_projection, lightView);

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
	deviceContext->DrawIndexed(m_indexCount, 0, 0);

	return;
}

void Terrain::RenderShadow(ID3D11DeviceContext* deviceContext, DirectX::SimpleMath::Matrix* positionshadowView)
{
	setLocation();

	lightView = positionshadowView;

	shader.EnableShader(deviceContext);

	shader.SetShaderParameters(deviceContext, m_world, positionshadowView, m_projection, m_Light,
		sandTexture.Get(), sandNormalTexture.Get(), grassTexture.Get(), grassNormalTexture.Get(), snowTexture.Get(), snowNormalTexture.Get(), rockTexture.Get(), rockNormalTexture.Get(),
		camera->getPosition());

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
	deviceContext->DrawIndexed(m_indexCount, 0, 0);

	return;
}

DirectX::XMFLOAT3 Terrain::getHeightPoint(DirectX::XMFLOAT3 point)
{
	int index = 0;

	if (!(point.x >= -49 && point.x <= 49 && point.z >= -49 && point.z <= 49)) //Outside
		return DirectX::XMFLOAT3();
	else
	{
		point = DirectX::XMFLOAT3(point.x / 0.1f, point.y / 0.1, point.z / 0.1f); // Scale factor

		point.x += 500;
		point.z += 500;

		index = (float)(m_terrainWidth) * round(point.z) + round(point.x);
	}


	DirectX::XMFLOAT3 terrainPoint = DirectX::XMFLOAT3(m_heightMap[index].x*0.1, m_heightMap[index].y *0.1, m_heightMap[index].z*0.1);

	return terrainPoint;
}


bool Terrain::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	DirectX::SimpleMath::Vector3* normals;
	

	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new DirectX::SimpleMath::Vector3[(m_terrainDepth - 1) * (m_terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j<(m_terrainDepth - 1); j++)
	{
		for (i = 0; i<(m_terrainWidth - 1); i++)
		{
			index1 = (j * m_terrainDepth) + i;
			index2 = (j * m_terrainDepth) + (i + 1);
			index3 = ((j + 1) * m_terrainDepth) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainDepth - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for (j = 0; j< m_terrainDepth; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainDepth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainDepth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (m_terrainDepth - 1)))
			{
				index = (j * (m_terrainDepth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainDepth - 1)))
			{
				index = (j * (m_terrainDepth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainDepth) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}

void Terrain::Shutdown()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	delete meshTriangles;

	return;
}

bool Terrain::InitializeBuffers(ID3D11Device * device )
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int index, i, j;
	int index1, index2, index3, index4; //geometric indices. 

	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainDepth - 1) * 6;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	meshTriangles = new TriangleType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize the index to the vertex buffer.
	index = 0;

	for (j = 0; j<(m_terrainDepth - 1); j++)
	{
		for (i = 0; i<(m_terrainWidth - 1); i++)
		{
			index1 = (m_terrainDepth * j) + i;          // Bottom left.
			index2 = (m_terrainDepth * j) + (i + 1);      // Bottom right.
			index3 = (m_terrainDepth * (j + 1)) + i;      // Upper left.
			index4 = (m_terrainDepth * (j + 1)) + (i + 1);  // Upper right.

															 

			if (i + j % 2 == 0)
			{
				meshTriangles[index].position0 = DirectX::SimpleMath::Vector3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);

				vertices[index].position = DirectX::SimpleMath::Vector3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
				vertices[index].normal = DirectX::SimpleMath::Vector3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
				vertices[index].texture = DirectX::SimpleMath::Vector2(m_heightMap[index3].u, m_heightMap[index3].v);
				indices[index] = index;
				index++;

				// Upper right.
				meshTriangles[index].position1 = DirectX::SimpleMath::Vector3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);

				vertices[index].position = DirectX::SimpleMath::Vector3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
				vertices[index].normal = DirectX::SimpleMath::Vector3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
				vertices[index].texture = DirectX::SimpleMath::Vector2(m_heightMap[index4].u, m_heightMap[index4].v);
				indices[index] = index;
				index++;

				// Bottom left.
				meshTriangles[index].position2 = DirectX::SimpleMath::Vector3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);

				vertices[index].position = DirectX::SimpleMath::Vector3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
				vertices[index].normal = DirectX::SimpleMath::Vector3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
				vertices[index].texture = DirectX::SimpleMath::Vector2(m_heightMap[index1].u, m_heightMap[index1].v);
				indices[index] = index;
				index++;

				// Bottom left.
				meshTriangles[index].position0 = DirectX::SimpleMath::Vector3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);

				vertices[index].position = DirectX::SimpleMath::Vector3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
				vertices[index].normal = DirectX::SimpleMath::Vector3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
				vertices[index].texture = DirectX::SimpleMath::Vector2(m_heightMap[index1].u, m_heightMap[index1].v);
				indices[index] = index;
				index++;

				// Upper right.
				meshTriangles[index].position1 = DirectX::SimpleMath::Vector3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);

				vertices[index].position = DirectX::SimpleMath::Vector3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
				vertices[index].normal = DirectX::SimpleMath::Vector3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
				vertices[index].texture = DirectX::SimpleMath::Vector2(m_heightMap[index4].u, m_heightMap[index4].v);
				indices[index] = index;
				index++;

				// Bottom right.
				meshTriangles[index].position2 = DirectX::SimpleMath::Vector3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);

				vertices[index].position = DirectX::SimpleMath::Vector3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
				vertices[index].normal = DirectX::SimpleMath::Vector3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
				vertices[index].texture = DirectX::SimpleMath::Vector2(m_heightMap[index2].u, m_heightMap[index2].v);
				indices[index] = index;
				index++;
			}
			else
			{
				// Upper right.
				meshTriangles[index].position0 = DirectX::SimpleMath::Vector3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);

				vertices[index].position = DirectX::SimpleMath::Vector3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
				vertices[index].normal = DirectX::SimpleMath::Vector3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
				vertices[index].texture = DirectX::SimpleMath::Vector2(m_heightMap[index4].u, m_heightMap[index4].v);
				indices[index] = index;
				index++;

				//Upper Left
				meshTriangles[index].position1 = DirectX::SimpleMath::Vector3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);

				vertices[index].position = DirectX::SimpleMath::Vector3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
				vertices[index].normal = DirectX::SimpleMath::Vector3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
				vertices[index].texture = DirectX::SimpleMath::Vector2(m_heightMap[index3].u, m_heightMap[index3].v);
				indices[index] = index;
				index++;

				// Bottom right.
				meshTriangles[index].position2 = DirectX::SimpleMath::Vector3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);

				vertices[index].position = DirectX::SimpleMath::Vector3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
				vertices[index].normal = DirectX::SimpleMath::Vector3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
				vertices[index].texture = DirectX::SimpleMath::Vector2(m_heightMap[index2].u, m_heightMap[index2].v);
				indices[index] = index;
				index++;

				// Upper right.
				meshTriangles[index].position0 = DirectX::SimpleMath::Vector3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);

				vertices[index].position = DirectX::SimpleMath::Vector3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
				vertices[index].normal = DirectX::SimpleMath::Vector3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
				vertices[index].texture = DirectX::SimpleMath::Vector2(m_heightMap[index3].u, m_heightMap[index3].v);
				indices[index] = index;
				index++;

				// Bottom left.
				meshTriangles[index].position1 = DirectX::SimpleMath::Vector3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);

				vertices[index].position = DirectX::SimpleMath::Vector3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
				vertices[index].normal = DirectX::SimpleMath::Vector3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
				vertices[index].texture = DirectX::SimpleMath::Vector2(m_heightMap[index1].u, m_heightMap[index1].v);
				indices[index] = index;
				index++;

				// Bottom left.
				meshTriangles[index].position2 = DirectX::SimpleMath::Vector3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);

				vertices[index].position = DirectX::SimpleMath::Vector3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
				vertices[index].normal = DirectX::SimpleMath::Vector3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
				vertices[index].texture = DirectX::SimpleMath::Vector2(m_heightMap[index2].u, m_heightMap[index2].v);
				indices[index] = index;
				index++;



			}
			
		}
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void Terrain::RenderBuffers(ID3D11DeviceContext * deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

float Terrain::createRandomHeight()
{
	return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 10));
}

void Terrain::SmoothAmplitude(float deltaTime, int direction)
{
	m_amplitude += direction * deltaTime * 5; //5 speed
	if (m_amplitude < 0) m_amplitude = 0;
}

bool Terrain::cameraIntersectPlane()
{

	return false;
}

bool Terrain::GenerateHeightMap(ID3D11Device* device)
{
	bool result;

	int index;
	float height = 0.0;

	//NOISE
	fnl_state  noise = fnlCreateState();
	noise.fractal_type = FNL_FRACTAL_FBM;
	noise.octaves = 3;
	noise.lacunarity = 2;
	noise.frequency = 0.005;
	noise.seed = rand() % 10000 + 1;     // randomrange 1 to 10000;

	int middleX = m_terrainDepth / 2;
	int middleZ = m_terrainWidth / 2;

	float topDistance = middleX * 3 / 10;
	float waterDistance = middleX - (middleX / 15);

	for (int j = 0; j< m_terrainDepth; j++)
	{
		for (int i = 0; i<m_terrainWidth; i++)
		{
			index = (m_terrainDepth * j) + i;
			//---------------SIMPLEX PERLIN NOISE HEIGHTS----------------
			float currentMaxHeight = maxHeight;
			float currentMinHeight = minHeight;

			double dist = std::sqrt(std::pow(i - middleX, 2) + std::pow(j - middleZ, 2));
			
			if (dist < topDistance)
			{
				currentMinHeight = 0;
			}
			else if (dist > waterDistance)
			{
				currentMaxHeight = 0;
			}
			else
			{
				float lerpValue = (dist - topDistance) / (waterDistance - topDistance);
				//lerpValue = 0.5f - 0.5f * std::cos(lerpValue * M_PI);

				currentMaxHeight = (1 - lerpValue) * maxHeight;
				currentMinHeight = lerpValue * minHeight;
			}

			m_heightMap[index].x = (float)i;
			m_heightMap[index].z = (float)j;

			float heightNormalized = fnlGetNoise3D(&noise, (FNLfloat)i, 0 ,(FNLfloat)j);
			float height = (heightNormalized + 1.0) * 0.5 * (currentMaxHeight - currentMinHeight) + currentMinHeight;
			m_heightMap[index].y = height;
		}
	}

	//-------------------------------------
	result = CalculateNormals();
	if (!result)
	{
		return false;
	}

	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}
}

bool Terrain::Update()
{


	return true; 
}

float* Terrain::GetWavelength()
{
	return &m_wavelength;
}

float* Terrain::GetAmplitude()
{
	return &m_amplitude;
}
float* Terrain::GetPAmplitude()
{
	return &mAmplitude;
}

float* Terrain::GetFrecuency()
{
	return &mFrequency;
}

float* Terrain::GetLacunarity()
{
	return &mLacunarity;
}

float* Terrain::GetPersistence()
{
	return &mPersistence;
}


void Terrain::setShadowMap(ID3D11ShaderResourceView* texture)
{
	shadowTexture = texture;
}

void Terrain::setLocation()
{
	*m_world = SimpleMath::Matrix::Identity;
	//prepare transform for second object.
	SimpleMath::Matrix newScale = SimpleMath::Matrix::CreateScale(scale.x, scale.y, scale.z);

	float toRadians = 0.017453; // PI / 180
	SimpleMath::Matrix newRotationX = SimpleMath::Matrix::CreateRotationX(rotation.x * toRadians);
	SimpleMath::Matrix newRotationY = SimpleMath::Matrix::CreateRotationY(rotation.y * toRadians);
	SimpleMath::Matrix newRotationZ = SimpleMath::Matrix::CreateRotationZ(rotation.z * toRadians);

	SimpleMath::Matrix newRotation = newRotationX * newRotationY * newRotationZ;

	SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(position.x, position.y, position.z);
	*m_world = (*m_world) * newScale * newRotation * newPosition;
}