#pragma once

#define START_ROT_SPEED 60.f

#define FAST_SPEED 15.f
#define SLOW_SPEED 5.f

#define MAX_ROT_SPEED 30.f
#define MIN_ROT_SPEED 90.f

#define ROT_PER_CLICK 10.f
struct InputCommands;

class Camera
{
public:
	Camera();
	~Camera();

	void							Update(double deltaTime, InputCommands* input);
	DirectX::SimpleMath::Matrix		getCameraMatrix();
	void							setPosition(DirectX::SimpleMath::Vector3 newPosition);
	DirectX::SimpleMath::Vector3	getPosition();
	DirectX::SimpleMath::Vector3	getForward();
	DirectX::SimpleMath::Vector3	getRight();
	void							setRotation(DirectX::SimpleMath::Vector3 newRotation);
	DirectX::SimpleMath::Vector3	getRotation();
	float							getMoveSpeed();
	float							getRotationSpeed();

	void checkCollision(DirectX::XMFLOAT3 height);

private:
	void HandleEvent(double deltaTime, InputCommands* input);

	void calculateCamera();

	DirectX::SimpleMath::Matrix		m_cameraMatrix;			//camera matrix to be passed out and used to set camera position and angle for wrestling
	DirectX::SimpleMath::Vector3	m_lookat;
	DirectX::SimpleMath::Vector3	m_position;
	DirectX::SimpleMath::Vector3	m_previousPosition;
	DirectX::SimpleMath::Vector3	m_forward;
	DirectX::SimpleMath::Vector3	m_right;
	DirectX::SimpleMath::Vector3	m_up;
	DirectX::SimpleMath::Vector3	m_orientation;			//vector storing pitch yaw and roll. 

	float	m_movSpeed;
	float	m_camRotRate;

	float cameraRadio = 1; // radio of camera collision
};

