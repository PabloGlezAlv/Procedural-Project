#pragma once
#define MAX_VOLUME 0.1f

#define RIGHT_LIMIT 4.5f
#define BUTTOM_LIMIT 4.5f

#define PAN_TIME 10.f

class Camera;
struct InputCommands;

class WavesSound
{
public:
	WavesSound(){}
	WavesSound(DirectX::AudioEngine* engine);
	~WavesSound();

	void update(float deltaTime, InputCommands* input);

	void setCamera(Camera* cam);

	void retrySound();
	
	bool getUnderWater();
private:
	bool finalHeight();

	Camera* camera;
	uint32_t                                                                m_audioEvent;
	float                                                                   m_audioTimerAcc;
	bool                                                                    m_retryDefault;

	DirectX::AudioEngine*                                  m_audEngine;

	DirectX::SoundEffect*                                  m_soundEffect;
	DirectX::SoundEffect*                                  m_soundEffectSplash;
	DirectX::SoundEffect*                                  m_soundEffectUnderWater;
	std::unique_ptr<DirectX::SoundEffectInstance>                        splashEffect;
	std::unique_ptr<DirectX::SoundEffectInstance>                        underWaterEffect;
	std::unique_ptr<DirectX::SoundEffectInstance>                        waterEffect;

	bool sand = false;
	bool underWater = false;

	float waterVolume = MAX_VOLUME;

	bool transitionDown = false;
	bool transitionUp = false;

	bool soundEffect = false;

	float panTimer = 0;
	bool panEffect = false;

	float time = 0;
};

