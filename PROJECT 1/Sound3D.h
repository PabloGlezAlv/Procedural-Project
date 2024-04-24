#pragma once

class Camera;
struct InputCommands;

class Sound3D
{
public:
	Sound3D() {}
	Sound3D(DirectX::AudioEngine* engine, std::string name, DirectX::AudioListener* listener, DirectX::XMFLOAT3 position
		, float maxDistance, float timeRepeat = 0);
	~Sound3D();

	void update(float deltaTime, InputCommands* input);

	void setCamera(Camera* cam);

	void retrySound();
private:
	bool finalHeight();

	DirectX::XMFLOAT3 position;

	Camera* camera;
	uint32_t                                                                m_audioEvent;
	float                                                                   m_audioTimerAcc;
	bool                                                                    m_retryDefault;

	DirectX::AudioEmitter  m_emitter;
	DirectX::AudioListener* m_listener;

	DirectX::AudioEngine* m_audEngine;

	DirectX::SoundEffect* m_soundEffect;
	std::unique_ptr<DirectX::SoundEffectInstance>                        soundEffect;

	float time = 0;

	float maxDistance = 0;

	bool repeatSound = false;
	float repeatTimer = 0;
	float repeatDuration = 0;
};

