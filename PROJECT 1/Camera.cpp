#include "pch.h"
#include "Camera.h"
#include "Input.h"

Camera::Camera()
{
	//initalise values. 
	//Orientation and Position are how we control the camera. 
	m_orientation.x = -90.0f;		//rotation around x - pitch
	m_orientation.y = 0.0f;		//rotation around y - yaw
	m_orientation.z = 0.0f;		//rotation around z - roll	//we tend to not use roll a lot in first person

	m_position.x = 0.0f;		//camera position in space. 
	m_position.y = 0.0f;
	m_position.z = 0.0f;

	//These variables are used for internal calculations and not set.  but we may want to queary what they 
	//externally at points
	m_lookat.x = 0.0f;		//Look target point
	m_lookat.y = 0.0f;
	m_lookat.z = 0.0f;

	m_forward.x = 0.0f;		//forward/look direction
	m_forward.y = 0.0f;
	m_forward.z = 0.0f;

	m_right.x = 0.0f;
	m_right.y = 0.0f;
	m_right.z = 0.0f;

	//
	m_movSpeed = SLOW_SPEED;
	m_camRotRate = START_ROT_SPEED;

	//force update with initial values to generate other camera data correctly for first update. 
	calculateCamera();
}


Camera::~Camera()
{
}

void Camera::HandleEvent(double deltaTime, InputCommands* input)
{
	m_previousPosition = m_position;

	//---------------CHANGE SPEED------------------
	if (input->mouseRight)
	{
		m_movSpeed -= SLOW_SPEED;
		if(m_movSpeed < SLOW_SPEED)
			m_movSpeed = SLOW_SPEED;
	}
	if (input->mouseLeft)
	{
		m_movSpeed += SLOW_SPEED;
		if (m_movSpeed > FAST_SPEED)
			m_movSpeed = FAST_SPEED;
	}
	

	//---------ROTATION----------------------
	m_orientation.y += input->deltaX * m_camRotRate * deltaTime;
	m_orientation.x += input->deltaY * m_camRotRate * deltaTime;

	if (m_orientation.x > 90)m_orientation.x =90;
	if (m_orientation.x < -90)m_orientation.x = -90;
	//-------MOVEMENT-------------------------
	if (input->movForward)
	{
		m_position += (m_forward * m_movSpeed * deltaTime);
	}
	if (input->movBack)
	{
		m_position -= (m_forward * m_movSpeed * deltaTime);
	}
	if (input->movRight)
	{
		m_position += (m_right* m_movSpeed * deltaTime);
	}
	if (input->movLeft)
	{
		m_position -= (m_right * m_movSpeed * deltaTime);
	}
	if (input->movUp)
	{
		m_position.y += m_movSpeed * deltaTime;
	}
	if (input->movDown)
	{
		m_position.y -= m_movSpeed * deltaTime;
	}
}

void Camera::calculateCamera()
{
	// parametric sphere equations matched to right-handed coordinate system
	// for yaw and pitch rotation
	float r = 3.1415f / 180.0f;
	m_forward.x = cos(m_orientation.x * r) * cos(m_orientation.y * r);
	m_forward.y = sin(m_orientation.x * r);
	m_forward.z = cos(m_orientation.x * r) * sin(m_orientation.y * r);

	m_forward.Normalize();

	//create right vector from look Direction
	m_forward.Cross(DirectX::SimpleMath::Vector3::UnitY, m_right);

	//update lookat point
	m_lookat = m_position + m_forward;

	//apply camera vectors and create camera matrix
	m_cameraMatrix = (DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_lookat, DirectX::SimpleMath::Vector3::UnitY));
}

void Camera::Update(double deltaTime, InputCommands* input)
{
	HandleEvent(deltaTime, input);

	calculateCamera();
}

DirectX::SimpleMath::Matrix Camera::getCameraMatrix()
{
	return m_cameraMatrix;
}

void Camera::setPosition(DirectX::SimpleMath::Vector3 newPosition)
{
	m_position = newPosition;
}

DirectX::SimpleMath::Vector3 Camera::getPosition()
{
	return m_position;
}

DirectX::SimpleMath::Vector3 Camera::getForward()
{
	return m_forward;
}

DirectX::SimpleMath::Vector3 Camera::getRight()
{
	return m_right;
}

void Camera::setRotation(DirectX::SimpleMath::Vector3 newRotation)
{
	m_orientation = newRotation;
}

DirectX::SimpleMath::Vector3 Camera::getRotation()
{
	return m_orientation;
}

float Camera::getMoveSpeed()
{
	return m_movSpeed;
}

float Camera::getRotationSpeed()
{
	return m_camRotRate;
}

void Camera::checkCollision(DirectX::XMFLOAT3 terrainHeight)
{
	if (terrainHeight.y + cameraRadio > m_position.y) //Camerabelow the terrain
	{
		m_position.y = terrainHeight.y + cameraRadio;

		calculateCamera();
	}
}
