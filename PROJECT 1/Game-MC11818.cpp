//
// Game.cpp
//

#include "pch.h"
#include "Game.h"


//toreorganise
#include <fstream>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
#ifdef DXTK_AUDIO
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{

	m_input.Initialise(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

	m_fullscreenRect.left = 0;
	m_fullscreenRect.top = 0;
	m_fullscreenRect.right = 800;
	m_fullscreenRect.bottom = 600;

	m_CameraViewRect.left = 500;
	m_CameraViewRect.top = 0;
	m_CameraViewRect.right = 800;
	m_CameraViewRect.bottom = 240;

	//setup light
	m_Light.setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setPosition(-7.f, 5.f, -12.f);
	m_Light.setDirection(-1.0f, -1.0f, 0.0f);

	//setup camera
	m_Camera01.setPosition(Vector3(0.0f, 0.0f, 4.0f));
	m_Camera01.setRotation(Vector3(0.0f, -90.0f, 0.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up. 

    shadowView = (DirectX::SimpleMath::Matrix::CreateLookAt(m_Light.getPosition(), DirectX::SimpleMath::Vector3(4, 6,-12), DirectX::SimpleMath::Vector3::UnitY));

#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
#endif
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	//take in input
	m_input.Update();								//update the hardware
	m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game
	
	//Update all game objects
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

	//Render all game content. 
    Render();

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

	
}



// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float deltaTime = timer.GetElapsedSeconds();

    m_Camera01.Update(deltaTime, &m_gameInputCommands);

    //testSystem->Update(m_deviceResources->GetD3DDeviceContext(), timer.GetElapsedSeconds());
    

    rotationAngle += 3.141592 / 180 * (ROTATION_SPEED) * deltaTime;
    DirectX::SimpleMath::Vector3 lightPosition = m_Light.getPosition();
    //-----------------ROTATE THE VIEW----------------------------

    DirectX::SimpleMath::Vector3 targetPosition(1, lightPosition.y, 6);

    float cosAngle = cos(rotationAngle);
    float sinAngle = sin(rotationAngle);
    float offsetX = (targetPosition.x - lightPosition.x) * cosAngle - (targetPosition.z - lightPosition.z) * sinAngle;
    float offsetZ = (targetPosition.x - lightPosition.x) * sinAngle + (targetPosition.z - lightPosition.z) * cosAngle;
    targetPosition.x = lightPosition.x + offsetX;
    targetPosition.z = lightPosition.z + offsetZ;

    shadowView = DirectX::SimpleMath::Matrix::CreateLookAt(lightPosition, targetPosition, DirectX::SimpleMath::Vector3::UnitY);

    lighthouse.Update(deltaTime);

    boat.Update(deltaTime);

    sand.Update(deltaTime);

    water.Update(deltaTime);


	m_view = m_Camera01.getCameraMatrix();
	m_world = Matrix::Identity;

    m_batchEffect->SetView(m_view);
    m_batchEffect->SetWorld(Matrix::Identity);

#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif

  
	if (m_input.Quit())
	{
		ExitGame();
	}
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{	
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTargetView = m_deviceResources->GetRenderTargetView();
	auto depthTargetView = m_deviceResources->GetDepthStencilView();

 //   // Draw Text to the screen
    m_sprites->Begin();
		m_font->DrawString(m_sprites.get(), L"DirectXTK Demo Window", XMFLOAT2(10, 10), Colors::Yellow);
    m_sprites->End();

    // Don't try to render anything before the first Update.
    m_deviceResources->PIXEndEvent();

    //Set Rendering states. 
	context->OMSetBlendState(customBlendState, nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(customRasterizerState);
	
    ShadowTexture();
	sceneToTexture();
    PostProcess();

	///////////////////////////////////////draw our sprite with the render texture displayed on it. 
    //------------------FULL SCREEN------------------------
	m_sprites->Begin();
	m_sprites->Draw(scenePPTexture->getShaderResourceView(), m_fullscreenRect);
	m_sprites->End();
    //------------------SMALL SCREEN------------------------
    //m_spriteswith the->Begin();
    //m_sprites->Draw(cameraShadowView->getShaderResourceView(), m_CameraViewRect);
    //m_sprites->End();

    // Show the new frame.
    m_deviceResources->Present();
}

void Game::sceneToTexture()
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTargetView = m_deviceResources->GetRenderTargetView();
	auto depthTargetView = m_deviceResources->GetDepthStencilView();
    auto viewport = m_deviceResources->GetScreenViewport();
	// Set the render target to be the render to texture.
	justSceneView->setRenderTarget(context);
	// Clear the render to texture.
	justSceneView->clearRenderTarget(context, 0.0f, 0.0f, 1.0f, 1.0f);

    //testSystem->Render(context);


    skybox.setPosition(m_Camera01.getPosition());
    context->OMSetDepthStencilState(m_states->DepthNone(), 0);
    skybox.setShadowMap(cameraShadowView->getDepthResourceView());
    skybox.Render(context);
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);


    boat.setShadowMap(cameraShadowView->getDepthResourceView());
    boat.Render(context);
    sand.setShadowMap(cameraShadowView->getDepthResourceView());
    sand.Render(context);

    lighthouse.setShadowMap(cameraShadowView->getDepthResourceView());
    lighthouse.Render(context);

    for (int i = 0; i < staticEntities.size(); i++)
    {
        staticEntities[i].setShadowMap(cameraShadowView->getDepthResourceView());
        staticEntities[i].Render(context);
    }

    //LAST TOP RENDER ALPHA
    water.setShadowMap(cameraShadowView->getDepthResourceView());
    water.Render(context);

    //Own geometry
    m_batchEffect->Apply(context);
    context->IASetInputLayout(m_batchInputLayout.Get());

    lighthouse.RenderCrystal();

	// Reset the render target back to the original back buffer and not the render to texture anymore.	
	context->OMSetRenderTargets(1, &renderTargetView, depthTargetView);

    context->RSSetViewports(1, &viewport);
}

void Game::ShadowTexture()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTargetView = m_deviceResources->GetRenderTargetView();
    auto depthTargetView = m_deviceResources->GetDepthStencilView();
    auto viewport = m_deviceResources->GetScreenViewport();
    // Set the render target to be the render to texture.
    cameraShadowView->setRenderTarget(context);
    // Clear the render to texture.
    cameraShadowView->clearRenderTarget(context, 0.0f, 0.0f, 1.0f, 1.0f);

    //testSystem->Render(context);

    skybox.setPosition(m_Camera01.getPosition());
    context->OMSetDepthStencilState(m_states->DepthNone(), 0);
    skybox.RenderShadow(context, &shadowView);
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

    boat.RenderShadow(context, &shadowView);

    sand.RenderShadow(context, &shadowView);

    //lighthouse.RenderShadow(context, &shadowView);

    for (int i = 0; i < staticEntities.size(); i++)
    {
        staticEntities[i].RenderShadow(context, &shadowView);
    }


    //LAST TOP RENDER ALPHA
    //water.RenderShadow(context, &shadowView);

    // Reset the render target back to the original back buffer and not the render to texture anymore.	
    context->OMSetRenderTargets(1, &renderTargetView, depthTargetView);

    context->RSSetViewports(1, &viewport);
}

void Game::PostProcess()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTargetView = m_deviceResources->GetRenderTargetView();
    auto depthTargetView = m_deviceResources->GetDepthStencilView();
    auto viewport = m_deviceResources->GetScreenViewport();

    // Set the render target to be the render to texture.
    scenePPTexture->setRenderTarget(context);
    // Clear the render to texture.
    scenePPTexture->clearRenderTarget(context, 0.0f, 0.0f, 1.0f, 1.0f);

    auto rView = justSceneView->getShaderResourceView();
    auto rShadowView = justSceneView->getDepthResourceView();
    m_sprites->Begin(SpriteSortMode_Immediate, nullptr, nullptr, nullptr, nullptr, [=]()
        {
            context->PSSetShader(pixelShaderDOF.Get(), nullptr, 0);

            context->PSSetShaderResources(0, 1, &rView);
            context->PSSetShaderResources(1, 1, &rShadowView);
        });
    m_sprites->Draw(rView, m_fullscreenRect);
    m_sprites->End();

    // Reset the render target back to the original back buffer and not the render to texture anymore.	
    context->OMSetRenderTargets(1, &renderTargetView, depthTargetView);
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
    context->RSSetState(m_states->CullClockwise());

    context->RSSetViewports(1, &viewport);
}
// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
    m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
    m_audEngine->Resume();
#endif
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);
    m_fxFactory = std::make_unique<EffectFactory>(device);
    m_sprites = std::make_unique<SpriteBatch>(context);

    m_batchEffect = std::make_unique<BasicEffect>(device);
    m_batchEffect->SetVertexColorEnabled(true);

    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

        DX::ThrowIfFailed(
            device->CreateInputLayout(VertexPositionColor::InputElements,
                VertexPositionColor::InputElementCount,
                shaderByteCode, byteCodeLength,
                m_batchInputLayout.ReleaseAndGetAddressOf())
        );
    }

    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

    //Set Rendering states. 
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    device->CreateBlendState(&blendDesc, &customBlendState);

    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
    rasterizerDesc.CullMode = D3D11_CULL_NONE; 
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.AntialiasedLineEnable = true;

    device->CreateRasterizerState(&rasterizerDesc, &customRasterizerState);

    //--------------Postprocessing-----------
	auto pixelShaderBuffer = DX::ReadData(L"DOF_ps.cso");
	device->CreatePixelShader(pixelShaderBuffer.data(), pixelShaderBuffer.size(), NULL, &pixelShaderDOF);

    //-------------------------------------
	//Initialise Render to texture
	justSceneView = new RenderTexture(device, 800, 600, 1.f, 100.f);	//for our rendering, We dont use the last two properties. but.  they cant be zero and they cant be the same. 
	scenePPTexture = new RenderTexture(device, 800, 600, 1.f, 100.f);
    cameraShadowView = new RenderTexture(device, 800, 600, 1.f, 100.f);

    testSystem = new ParticleSystem(device, 128);

    lighthouse = LightHouse(device, m_batch.get(), "lighthouse", "lightNormal", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01, ROTATION_SPEED);
    lighthouse.Initialize(DirectX::XMFLOAT3(-7, -3, -12), DirectX::XMFLOAT3(1.f, 1.f, 1.f), DirectX::XMFLOAT3(0.f, 0.f, 0.f));

    boat = Boat(device, "boat","lightNormal", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    boat.Initialize(DirectX::XMFLOAT3(-0.07f, -0.3f,-1.15f), DirectX::XMFLOAT3(0.9f, 0.9f, 0.9f), DirectX::XMFLOAT3(0.f, 135.f, 0.f));

    water = Sea(device, "water2","waves", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    water.Initialize(DirectX::XMFLOAT3(8.f, -0.3f, 12.f), DirectX::XMFLOAT3(1.f,1.f,1.f), DirectX::XMFLOAT3(0.f, 0.f, 0.f));

    sand = Sand(device, "sand", "sand", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    sand.Initialize(DirectX::XMFLOAT3(12.f, 0, 0), DirectX::XMFLOAT3(1.f, 1.f, 1.f), DirectX::XMFLOAT3(0.f, 0.f, 0.f));

    StaticEntity dock = StaticEntity(device, "dock", "lightNormal", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    dock.Initialize(DirectX::XMFLOAT3(3.f, -1.6f, 6.f), DirectX::XMFLOAT3(0.5, 0.5, 0.5), DirectX::XMFLOAT3(0, 90, 0));

    staticEntities.push_back(dock);

    StaticEntity rocks = StaticEntity(device, "bigRock", "lightNormal", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    rocks.Initialize(DirectX::XMFLOAT3(15.f, 0.f, -8.f), DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f), DirectX::XMFLOAT3(-60, 50, 0));

    staticEntities.push_back(rocks);

    StaticEntity rocks2 = StaticEntity(device, "bigRock", "lightNormal", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    rocks2.Initialize(DirectX::XMFLOAT3(11.f, -1.f, 12.f), DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f), DirectX::XMFLOAT3(-15, 90, -50));

    staticEntities.push_back(rocks2);

    //--------------------------------------SEA ROCKS--------------------------------------------------------------------
    StaticEntity rocks3 = StaticEntity(device, "bigRock", "lightNormal", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    rocks3.Initialize(DirectX::XMFLOAT3(0.9, -8, -14), DirectX::XMFLOAT3(0.6, 0.6, 0.6), DirectX::XMFLOAT3(100, 0, 100));

    staticEntities.push_back(rocks3);

    StaticEntity rocks4 = StaticEntity(device, "bigRock", "lightNormal", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    rocks4.Initialize(DirectX::XMFLOAT3(0.9, -8.3, 14), DirectX::XMFLOAT3(0.6, 0.6, 0.6), DirectX::XMFLOAT3(100, 0, 100));

    staticEntities.push_back(rocks4);

    StaticEntity rocks5 = StaticEntity(device, "bigRock", "lightNormal", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    rocks5.Initialize(DirectX::XMFLOAT3(1.5, -1.9, -13), DirectX::XMFLOAT3(0.3, 0.3, 0.3), DirectX::XMFLOAT3(90, 0, 270));

    staticEntities.push_back(rocks5);


    StaticEntity rocks6 = StaticEntity(device, "bigRock", "lightNormal", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    rocks6.Initialize(DirectX::XMFLOAT3(1.3, -1.5, 5.5), DirectX::XMFLOAT3(0.2, 0.2, 0.2), DirectX::XMFLOAT3(90, 0, 270));

    staticEntities.push_back(rocks6);

    StaticEntity rocks7 = StaticEntity(device, "bigRock", "lightNormal", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    rocks7.Initialize(DirectX::XMFLOAT3(-10, -7, 13), DirectX::XMFLOAT3(0.45, 0.45, 0.45), DirectX::XMFLOAT3(100, 180, 170));

    staticEntities.push_back(rocks7);

    StaticEntity box = StaticEntity(device, "box", "lightNormal", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    box.Initialize(DirectX::XMFLOAT3(6, -5, 0), DirectX::XMFLOAT3(150, 120, 105), DirectX::XMFLOAT3(0, 90, 0));

    staticEntities.push_back(box);

    //------------------------------------------------------------------------------------------------------------------------------

    skybox = StaticEntity(device, "skybox", "skybox", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    skybox.Initialize(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(50, 50, 50), DirectX::XMFLOAT3(0, 0, 0));

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        100.0f
    );

    m_batchEffect->SetProjection(m_projection);
}


void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_batchEffect.reset();
    m_font.reset();
	m_batch.reset();
	m_testmodel.reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
