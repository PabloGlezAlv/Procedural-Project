#pragma once
#include <vector>

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Shader.h"
#include "modelclass.h"
#include "Light.h"
#include "Input.h"
#include "Camera.h"
#include "RenderTexture.h"
#include "ParticleSystem.h"
#include "Sea.h"
#include "Sand.h"
#include "Boat.h"
#include "StaticEntity.h"
#include "LightHouse.h"
#include "Crystal.h"
#include "WavesSound.h"
#include "Sound3D.h"
#include "Terrain.h"
#include "CircularMaze.h"
#include "PostProcessingManager.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    void NewAudioDevice();


    // Properties
    void GetDefaultSize( int& width, int& height ) const;
	
private:

    float rotationAngle = 270.0f;
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	}; 
    struct CameraBufferType
    {
        DirectX::SimpleMath::Vector3 camPosition;
    };

    void Update(DX::StepTimer const& timer);
    void Render();

	void sceneToTexture();
	void ShadowTexture();
    void PostProcess();

    void Clear();
    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

	//input manager. 
	Input									m_input;
	InputCommands							m_gameInputCommands;

    // DirectXTK objects.
    std::unique_ptr<DirectX::CommonStates>                                  m_states;
    std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;	
    std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
    std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
    std::unique_ptr<DirectX::SpriteFont>                                    m_font;

	// Scene Objects
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;
	std::unique_ptr<DirectX::GeometricPrimitive>                            m_testmodel;

	//lights
	Light																	m_Light;

	//Cameras
	Camera																	m_Camera01;

    //Postprocessing
    PostProcessingManager ppManager;

	//RenderTextures
	RenderTexture*															justSceneView;
    RenderTexture*                                                          cameraShadowView;
    RenderTexture*                                                          scenePPTexture;

	RECT																	m_fullscreenRect;
	RECT																	m_CameraViewRect;
	
    //Particle system
    ParticleSystem* testSystem;
    
    //BlendState - Enable alpha
    ID3D11BlendState* customBlendState = nullptr;

    //RasterizerState - Enable both faces
    ID3D11RasterizerState* customRasterizerState = nullptr;

    //Boat boat;
    Sea water;
    //Sand sand;
    //LightHouse lighthouse;
    std::vector<StaticEntity> staticEntities;

    StaticEntity skybox;

    //Own geometry


    //Shadow effect
    DirectX::SimpleMath::Matrix                                             shadowView;

    std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
    std::unique_ptr<DirectX::WaveBank>                                      m_waveBank;
   // std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect;
   // std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect1;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect2;

    


    //uint32_t                                                                m_audioEvent;
    //float                                                                   m_audioTimerAcc;

    //bool                                                                    m_retryDefault;


    DirectX::SimpleMath::Matrix                                             m_world;
    DirectX::SimpleMath::Matrix                                             m_view;
    DirectX::SimpleMath::Matrix                                             m_projection;

    //DirectX::AudioEmitter  m_emitter;
    DirectX::AudioListener* m_listener;

    int updateTimer = 0;

    Terrain terrain;
    CircularMaze maze;
};