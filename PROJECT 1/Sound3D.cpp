#include "pch.h"
#include <codecvt>

#include "Sound3D.h"
#include "Camera.h"
#include "Input.h"

Sound3D::Sound3D(DirectX::AudioEngine* engine, std::string name, DirectX::AudioListener* listener, DirectX::XMFLOAT3 pos,
    float mDistance, float timeRepeat)
{
    m_audEngine = engine;
    m_listener = listener;
    position = pos;
    maxDistance = mDistance;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring wideName = converter.from_bytes(name);
    const wchar_t* waveFileName = wideName.c_str();

    m_soundEffect = new DirectX::SoundEffect(m_audEngine, waveFileName);

    soundEffect = m_soundEffect->CreateInstance(DirectX::SOUND_EFFECT_INSTANCE_FLAGS::SoundEffectInstance_Use3D);

    if(timeRepeat == 0)
        soundEffect->Play(true);
    else
    {
        repeatSound = true;
        soundEffect->Play(false);
        repeatDuration = timeRepeat;
    }


}

Sound3D::~Sound3D()
{
    delete m_soundEffect;
}


void Sound3D::update(float deltaTime, InputCommands* input)
{
    time += deltaTime;
    repeatTimer += deltaTime;

    if (repeatSound && repeatTimer >= repeatDuration)
    {
        soundEffect->Stop();
        soundEffect->Play(false);
        repeatTimer = 0;
    }

    m_emitter.Update(DirectX::XMVectorSet(position.x, position.y, position.z, 1), camera->getForward(), deltaTime);

    float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(camera->getPosition() - position));


    if (soundEffect && distance < maxDistance)
    {
        soundEffect->SetVolume(1 - (distance/ maxDistance));
        soundEffect->Apply3D(*m_listener, m_emitter);
    }
    else
    {
        soundEffect->SetVolume(0);
    }

    m_audioTimerAcc -= deltaTime;
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                soundEffect->Play(true);
            }
        }
    }
}

void Sound3D::setCamera(Camera* cam)
{
    camera = cam;
}

void Sound3D::retrySound()
{
    m_audioTimerAcc = 1.f;
    m_retryDefault = true;
}

bool Sound3D::finalHeight()
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