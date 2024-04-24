#pragma once
#include <vector>
#include "Light.h"
#include "Camera.h"
#include "TerrainShader.h"
using namespace DirectX;

//https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf

#define EPSILON 0.000001
#define CROSS(dest, v1, v2) \
    do { \
        dest[0] = v1[1] * v2[2] - v1[2] * v2[1]; \
        dest[1] = v1[2] * v2[0] - v1[0] * v2[2]; \
        dest[2] = v1[0] * v2[1] - v1[1] * v2[0]; \
    } while (0)

#define DOT(v1, v2) (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2])
#define SUB(dest, v1, v2) \
    do { \
        dest[0] = v1[0] - v2[0]; \
        dest[1] = v1[1] - v2[1]; \
        dest[2] = v1[2] - v2[2]; \
    } while (0)

struct StaticObjects
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
	std::string name;
	StaticObjects(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 sca, DirectX::XMFLOAT3 rot, std::string n)
	{
		position = pos;
		rotation = rot;
		scale = sca;
		name = n;
	}
};

class Terrain
{
private:
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
	};
	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};
	struct TriangleType
	{
		DirectX::SimpleMath::Vector3 position0;
		DirectX::SimpleMath::Vector3 position1;
		DirectX::SimpleMath::Vector3 position2;
	};
public:
	Terrain(){}
	Terrain(ID3D11Device* device, Light* light,
		DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view,
		DirectX::SimpleMath::Matrix* projection, Camera* cam);
	~Terrain();


	bool Initialize(ID3D11Device*, int terrainWidth, int terrainHeight, int terrainDepth, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation);

	void Render(ID3D11DeviceContext*);
	bool GenerateHeightMap(ID3D11Device*);

	bool Update();
	float* GetWavelength();

	float* GetAmplitude();

	float* GetPAmplitude();
	float* GetFrecuency();
	float* GetLacunarity();
	float* GetPersistence();

	void RenderShadow(ID3D11DeviceContext* deviceContext, DirectX::SimpleMath::Matrix* positionshadowView);

	DirectX::XMFLOAT3 getHeightPoint(DirectX::XMFLOAT3 point);

	void setShadowMap(ID3D11ShaderResourceView* texture);

	//-------------------------------------------------
	void SmoothAmplitude(float deltaTime, int direction);
	//-------------------------------------------------

	bool cameraIntersectPlane();

	std::vector<StaticObjects> getTrees() { return treeData; }

private:
	void setLocation();

	float mFrequency = 1;
	float mAmplitude = 1;
	float mLacunarity = 2;
	float mPersistence = 0.5;

	bool CalculateNormals();
	void Shutdown();
	void ShutdownBuffers();
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	///--------------------------------------------------------
	float createRandomHeight();
	//---------------------------------------------------------


private:
	TriangleType* meshTriangles;
	bool m_terrainGeneratedToggle;
	int m_terrainWidth, m_terrainDepth;
	ID3D11Buffer * m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	float m_frequency, m_amplitude, m_wavelength;
	HeightMapType* m_heightMap;

	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;

	float dir = 1;

	float maxHeight = 150.0f;
	float minHeight = -100.0f;


	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotation;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        snowTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        snowNormalTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        sandTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        sandNormalTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        grassTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        grassNormalTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        rockTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        rockNormalTexture;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        shadowTexture;

	DirectX::SimpleMath::Matrix* m_world;
	DirectX::SimpleMath::Matrix* m_view;
	DirectX::SimpleMath::Matrix* m_projection;

	DirectX::SimpleMath::Matrix* lightView;

	Light* m_Light;

	Camera* camera;

	TerrainShader shader;

	std::vector<StaticObjects> treeData;
};

