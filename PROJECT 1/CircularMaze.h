#pragma once
#include <vector>
#include "StaticEntity.h"

#define STOPTIME 10.0f
#define ANGLES_SECOND 3.0f
#define PLAYER_ANGLES_SECOND 10.0f
#define CIRCLES_DISTANCE 5.0f

struct InputCommands;

struct roomInfo {
	float rotationY = 0;
	float positionX = 0;
	float positionZ = 0;
	bool right = false;
	bool left = false;
	bool outside = false;
	bool inside = false;

	int visited = 0;
};

class CircularMaze
{
public:
	CircularMaze(){}
	~CircularMaze();
	CircularMaze(int size, int nCircles, ID3D11Device* device, Light* light, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view,
		DirectX::SimpleMath::Matrix* projection, Camera* cam);

	void Update(float deltaTime, InputCommands* input);
	void Render(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture);
	void RenderShadow(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* positionshadowView);

	int getPoints();
private:
	enum direction{none, right, left, top, down};

	bool isValid(int x, int y);
	void GenerateDistribution(int x, int y);
	void CreateObjects(ID3D11Device* device, Light* light, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view,
		DirectX::SimpleMath::Matrix* projection, Camera* cam);
	void HandleEvent(InputCommands* input);
	
	void ReDoConections();

	void ChangeButtonPosition();

	std::vector<std::vector<roomInfo>> maze;
	int mazeSize;
	int circles;

	bool rotating = false;
	

	int indexStart = 0;
	int indexEnd = 0;

	float timer = 0;
	std::vector<float> angles;
	std::vector<int> speed;
	std::vector<float> startAngles;

	std::vector<StaticEntity> mazeBlocks;
	std::vector<StaticEntity> mazeCorridors;

	StaticEntity player;
	StaticEntity button;
	StaticEntity limit;

	int buttonCircle = 0;
	int buttonRotation = 0;

	int playerCircle = 0;
	int playerRotation = 0;

	direction playerDir = none;
	bool playerMoving = false;
	float startRotation = 0;
	float playerCenterRotation = 0;

	float startX = 0;

	int points = 0;
};

