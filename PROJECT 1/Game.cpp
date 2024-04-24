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

    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }

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
	m_fullscreenRect.right = 1280;
	m_fullscreenRect.bottom = 720;

	m_CameraViewRect.left = 500;
	m_CameraViewRect.top = 0;
	m_CameraViewRect.right = 800;
	m_CameraViewRect.bottom = 240;

	//setup light
	m_Light.setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setPosition(0.f, 30.f, 0.f);
	m_Light.setDirection(-1.0f, -1.0f, 0.0f);


	//setup camera
	m_Camera01.setPosition(Vector3(15.0f, 20.0f, 10.0f));
	m_Camera01.setRotation(Vector3(0.0f, 0, 0.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up. 

    shadowView = (DirectX::SimpleMath::Matrix::CreateLookAt(m_Light.getPosition(), DirectX::SimpleMath::Vector3(4, 15,25), DirectX::SimpleMath::Vector3::UnitY));


    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_listener = new DirectX::AudioListener();
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


    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
       /* soundWave->retrySound();
        crystalSound->retrySound();*/
    }
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float deltaTime = timer.GetElapsedSeconds();

    //Post processing effects
    ppManager.HandleEvent(&m_gameInputCommands);

    m_Camera01.Update(deltaTime, &m_gameInputCommands);

    m_Camera01.checkCollision(terrain.getHeightPoint(m_Camera01.getPosition()));


    m_listener->Update(DirectX::XMVectorSet(m_Camera01.getPosition().x, m_Camera01.getPosition().y, m_Camera01.getPosition().z, 1), DirectX::SimpleMath::Vector3::Up, deltaTime);

    water.Update(deltaTime);

    maze.Update(deltaTime, &m_gameInputCommands);

	m_view = m_Camera01.getCameraMatrix();
	m_world = Matrix::Identity;

    m_batchEffect->SetView(m_view);
    m_batchEffect->SetWorld(Matrix::Identity);


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




    // Don't try to render anything before the first Update.
    m_deviceResources->PIXEndEvent();

    //Set Rendering states. 
	context->OMSetBlendState(customBlendState, nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(customRasterizerState);
	
    ShadowTexture();
	sceneToTexture();
    PostProcess();

    //------------------FULL SCREEN------------------------
	m_sprites->Begin();
	m_sprites->Draw(scenePPTexture->getShaderResourceView(), m_fullscreenRect);
    
    std::string nameColorT = "Buttons pressed: " + std::to_string(maze.getPoints());
    std::wstring wideString = std::wstring(nameColorT.begin(), nameColorT.end());
    const wchar_t* wcharPtr = wideString.c_str();
    m_font->DrawString(m_sprites.get(), wcharPtr, XMFLOAT2(10, 10), Colors::Yellow);
	m_sprites->End();
    //------------------SMALL SCREEN------------------------
    //m_sprites->Begin();
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

    //   // Draw Text to the screen

    skybox.setPosition(m_Camera01.getPosition());
    context->OMSetDepthStencilState(m_states->DepthNone(), 0);
    skybox.setShadowMap(cameraShadowView->getDepthResourceView());
    skybox.Render(context);
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);


    for (int i = 0; i < staticEntities.size(); i++)
    {
        staticEntities[i].setShadowMap(cameraShadowView->getDepthResourceView());
        staticEntities[i].Render(context);
    }

    terrain.setShadowMap(cameraShadowView->getDepthResourceView());
    terrain.Render(context);

    maze.Render(context, cameraShadowView->getDepthResourceView());

    //LAST TOP RENDER ALPHA
    water.setShadowMap(cameraShadowView->getDepthResourceView());
    water.Render(context);

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

    for (int i = 0; i < staticEntities.size(); i++)
    {
        staticEntities[i].RenderShadow(context, &shadowView);
    }

    maze.RenderShadow(context, &shadowView);

    terrain.RenderShadow(context, &shadowView);

    //LAST TOP RENDER ALPHA
    water.RenderShadow(context, &shadowView);

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
            //Set the postprocces effect
            ppManager.Render(context, rView, rShadowView);

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

    m_audEngine->Suspend();

}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();


    m_audEngine->Resume();

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


void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        /*soundWave->retrySound();
        crystalSound->retrySound();*/
    }
}


// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 1280;
    height = 720;
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
    rasterizerDesc.FrontCounterClockwise = true;
    rasterizerDesc.AntialiasedLineEnable = true;

    device->CreateRasterizerState(&rasterizerDesc, &customRasterizerState);
    
    //--------------Postprocessing-----------

    ppManager = PostProcessingManager(device);

    //-------------------------------------
	//Initialise Render to texture
	justSceneView = new RenderTexture(device, 1280, 720, 1.f, 100.f);	//for our rendering, We dont use the last two properties. but.  they cant be zero and they cant be the same. 
	scenePPTexture = new RenderTexture(device, 1280, 720, 1.f, 100.f);
    cameraShadowView = new RenderTexture(device, 1280, 720, 1.f, 100.f);

    water = Sea(device, "water3","waves", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    water.Initialize(DirectX::XMFLOAT3(15.f, 0.4, 15.0f), DirectX::XMFLOAT3(1.5f,1.5f,1.5f), DirectX::XMFLOAT3(0.f, 0.f, 0.f));

    //------------------------------------------------------------------------------------------------------------------------------

    skybox = StaticEntity(device, "skybox", "skybox", &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    skybox.Initialize(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(50, 50, 50), DirectX::XMFLOAT3(0, 0, 0));

    //--------------------------------CIRCULAR MAZE------------------------------------------

    maze = CircularMaze(8,6, device, &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);

    //--------------------------------TERRAIN------------------------------------------
    int terrainWidth = 1000;

    terrain = Terrain(device, &m_Light, &m_world, &m_view, &m_projection, &m_Camera01);
    terrain.Initialize(device, terrainWidth, terrainWidth, 150, DirectX::XMFLOAT3(-terrainWidth/10/2, 0, -terrainWidth / 10 / 2), DirectX::XMFLOAT3(0.1, 0.1, 0.1), DirectX::XMFLOAT3(0, 0, 0));
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
