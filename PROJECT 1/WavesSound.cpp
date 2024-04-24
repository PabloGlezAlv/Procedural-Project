#include "pch.h"
#include "WavesSound.h"
#include "Camera.h"
#include "Input.h"

WavesSound::WavesSound(DirectX::AudioEngine* engine)
{
	m_audEngine = engine;

    m_soundEffect = new DirectX::SoundEffect(m_audEngine, L"Assets/Music/wavesSound.wav");
    m_soundEffectUnderWater = new DirectX::SoundEffect(m_audEngine, L"Assets/Music/underWater.wav");

    m_soundEffectSplash = new DirectX::SoundEffect(m_audEngine, L"Assets/Music/splash.wav");



    splashEffect = m_soundEffectSplash->CreateInstance();

    underWaterEffect = m_soundEffectUnderWater->CreateInstance();
    //underWaterEffect->Play(true);
    //underWaterEffect->SetVolume(0);
    waterEffect = m_soundEffect->CreateInstance();
    waterEffect->Play(true);
}

WavesSound::~WavesSound()
{
    delete m_soundEffect;
}


void WavesSound::update(float deltaTime, InputCommands* input)
{
    if (input->H) //Recover ears
    {
        if (PAN_TIME - panTimer > 1)
        {
            panTimer = PAN_TIME - 1;
        }
    }

    if (time == 0)
    {

    }

    time += deltaTime;

    DirectX::XMFLOAT3 cameraPos = camera->getPosition();

    if (!underWater)
    {
        float volume = (cameraPos.x - RIGHT_LIMIT) / 200;

        if (volume > MAX_VOLUME) volume = MAX_VOLUME;
        else if (volume < 0) volume = 0;

        float finalVolume = MAX_VOLUME - volume;

        waterEffect->SetVolume(finalVolume);
    }
    else
    {
        float volume = (cameraPos.y + BUTTOM_LIMIT) / BUTTOM_LIMIT / 10;

        if (volume > MAX_VOLUME) volume = MAX_VOLUME;
        else if (volume < 0) volume = 0;

        underWaterEffect->SetVolume(volume);
    }

    bool goingUnder = finalHeight();

    if (goingUnder && !transitionDown && !underWater)
    {
        transitionDown = true;
        transitionUp = false;
        
        splashEffect->SetPan(0.5);
        waterEffect->SetPan(0.5);
        underWaterEffect->SetPan(0.5);
        panEffect = true;

        splashEffect->Stop();
        splashEffect->Play();
        soundEffect = false;
    }
    if (!goingUnder && !transitionUp && underWater)
    {
        transitionUp = true;
        transitionDown = false;
        
        splashEffect->Stop();
        splashEffect->Play();
        soundEffect = false;
    }

    if (transitionDown)
    {
        if (splashEffect->GetState() == DirectX::SoundState::PLAYING)
        {
            underWater = true;
            transitionDown = false;
            waterEffect->Stop();
            underWaterEffect->Play(true);
        }
    }

    if (transitionUp)
    {
        if (splashEffect->GetState() == DirectX::SoundState::PLAYING)
        {
            underWater = false;
            transitionUp = false;
            waterEffect->Play(true);
            underWaterEffect->Stop();
        }
    }


    if (panEffect)
    {
        panTimer += deltaTime;
        if (panTimer > PAN_TIME)
        {
            panTimer = 0;
            panEffect = false;

            splashEffect->SetPan(0);
            waterEffect->SetPan(0);
            underWaterEffect->SetPan(0);
        }
    }

    m_audioTimerAcc -= deltaTime;
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                if(!underWater)
                    waterEffect->Play(true);
                else
                    underWaterEffect->Play(true);
            }
        }
    }
}

void WavesSound::setCamera(Camera* cam)
{
    camera = cam;
}

void WavesSound::retrySound()
{
    m_audioTimerAcc = 1.f;
    m_retryDefault = true;
}
bool WavesSound::getUnderWater()
{
    return underWater;
}

bool WavesSound::finalHeight()
{
    float s_X = 0;
    float s_Y = 0;
    float s_Z = 0;

    DirectX::XMFLOAT3 cameraPos = camera->getPosition();

    DirectX::XMFLOAT2 X0 = DirectX::XMFLOAT2(cameraPos.x, cameraPos.z);
    float height = 0;

    for (int i = 0; i < 3; i++)
    {
        DirectX::XMFLOAT2 K = { 1, static_cast<float>(i) };
        float a = 0.01; // a is amplitude

        DirectX::XMFLOAT2 X = { X0.x - K.x * a * (float)std::sin((K.x * X0.x + K.y * X0.y) - 0.45 * time * 10),
                    X0.y - K.y * a * (float)std::sin((K.x * X0.x + K.y * X0.y) - 0.45 * time * 10) };

        float y = a * std::cos((K.x * X0.x + K.y * X0.y) - 0.45 * time * 10);

        s_X += X.x / 64 - 0.5f;
        s_Y += X.y / 64 - 0.5f;
        s_Z += y;
    }
    DirectX::XMFLOAT4 finalPos;
    finalPos.x = s_X * 7.5f;
    finalPos.z = s_Y * 7.5f;
    finalPos.y = s_Z * 7.5f;
    finalPos.w = 1;

    return (finalPos.y > cameraPos.y);
}