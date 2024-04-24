#include "pch.h"
#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(ID3D11Device* device, int maxParticles, DirectX::XMMATRIX* w,
    DirectX::XMMATRIX* v, DirectX::XMMATRIX* p) {

    world = w;
    view = v;
    projection = p;

    startpoint = DirectX::XMFLOAT4(0,1,0,1);
    endpoint = DirectX::XMFLOAT4(0,10,0,1);

    //---------------------TEXTURE-----------------------------

    std::string nameColorT = "Assets/Textures/boat.dds";
    std::wstring wideString = std::wstring(nameColorT.begin(), nameColorT.end());
    const wchar_t* wcharPtr = wideString.c_str();
    CreateDDSTextureFromFile(device, wcharPtr, nullptr, colorTexture.ReleaseAndGetAddressOf());

    //-------------------PIXEL SHADER--------------------------------

    auto pixelShaderBuffer = DX::ReadData(L"particle_ps.cso");
    HRESULT error = device->CreatePixelShader(pixelShaderBuffer.data(), pixelShaderBuffer.size(), NULL, &m_pixelShader);

    //-------------------VERTEX SHADER--------------------------------

    auto vertexShaderBuffer = DX::ReadData(L"particle_vs.cso");
    error = device->CreateVertexShader(vertexShaderBuffer.data(), vertexShaderBuffer.size(), NULL, &m_vertexShader);

    // Crear el Input Layout
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "SV_VertexID", 0, DXGI_FORMAT_R32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    unsigned int numElements;
    numElements = sizeof(layoutDesc) / sizeof(layoutDesc[0]);

    // Create the vertex input layout.
    device->CreateInputLayout(layoutDesc, numElements, vertexShaderBuffer.data(), vertexShaderBuffer.size(), &m_layout);

    //-------------------GEOMETRY SHADER--------------------------------
    
    auto geometryShaderBuffer = DX::ReadData(L"particle_gs.cso");
    error = device->CreateGeometryShader(geometryShaderBuffer.data(), geometryShaderBuffer.size(), NULL, &m_geometryShader);



    //-------------------COMPUTE SHADER--------------------------------

    auto computeShaderBuffer = DX::ReadData(L"particle_cs.cso");
    error = device->CreateComputeShader(computeShaderBuffer.data(), computeShaderBuffer.size(), NULL, &m_computeShader);

    D3D11_BUFFER_DESC newSimulationBufferDesc;
    newSimulationBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    newSimulationBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    newSimulationBufferDesc.CPUAccessFlags = 0;
    newSimulationBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    newSimulationBufferDesc.StructureByteStride = sizeof(Particle);
    newSimulationBufferDesc.ByteWidth = sizeof(Particle) * MAX_PARTICLES; 
    device->CreateBuffer(&newSimulationBufferDesc, nullptr, &newSimulationBuffer);

    D3D11_BUFFER_DESC currentSimulationBufferDesc;
    currentSimulationBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    currentSimulationBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    currentSimulationBufferDesc.CPUAccessFlags = 0;
    currentSimulationBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    currentSimulationBufferDesc.StructureByteStride = sizeof(Particle);
    currentSimulationBufferDesc.ByteWidth = sizeof(Particle) * MAX_PARTICLES;

    device->CreateBuffer(&currentSimulationBufferDesc, nullptr, &currentSimulationBuffer);

    //------------------------SHADER RESOURCE VIEW-------------------------

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Use the appropriate format
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementWidth = MAX_PARTICLES; // Specify the number of elements in the buffer;
    srvDesc.Buffer.ElementOffset = 0;

    device->CreateShaderResourceView(currentSimulationBuffer, &srvDesc, &resourceview);

    //--------------------UNORDERED ACCES VIEW------------------------------

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = DXGI_FORMAT_UNKNOWN; // Use the appropriate format
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.NumElements = 512;// Specify the number of elements in the buffer;
    uavDesc.Buffer.Flags = 0;

    device->CreateUnorderedAccessView(newSimulationBuffer, &uavDesc, &uAccesView);

    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Use the appropriate format
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementWidth = MAX_PARTICLES;

    device->CreateShaderResourceView(newSimulationBuffer, &srvDesc, &NewSimulationStateSRV);
    device->CreateShaderResourceView(currentSimulationBuffer, &srvDesc, &CurrentSimulationStateSRV);

    //-----------------BUFFERS--------------------------------

    D3D11_BUFFER_DESC	matrixBufferDesc;

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;
    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);


    D3D11_BUFFER_DESC	particleBufferDesc;
    particleBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    particleBufferDesc.ByteWidth = sizeof(MatrixBufferType); 
    particleBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    particleBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    particleBufferDesc.MiscFlags = 0;
    particleBufferDesc.StructureByteStride = 0;
    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    device->CreateBuffer(&particleBufferDesc, NULL, &particleBuffer);

    D3D11_BUFFER_DESC	simulationBufferDesc;
    simulationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    simulationBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    simulationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    simulationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    simulationBufferDesc.MiscFlags = 0;
    simulationBufferDesc.StructureByteStride = 0;
    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    device->CreateBuffer(&simulationBufferDesc, NULL, &simulationBuffer);

    D3D11_BUFFER_DESC	nParticlesBufferDesc;
    nParticlesBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    nParticlesBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    nParticlesBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    nParticlesBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    nParticlesBufferDesc.MiscFlags = 0;
    nParticlesBufferDesc.StructureByteStride = 0;
    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    device->CreateBuffer(&nParticlesBufferDesc, NULL, &particleCountBuffer);

    //---------------CREATE STATES---------------------
    //---SAMPLE
    D3D11_SAMPLER_DESC	samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the texture sampler state.
    device->CreateSamplerState(&samplerDesc, &m_wrapState);
}

ParticleSystem::~ParticleSystem() {
    // Liberar recursos
    if (particleBuffer) {
        particleBuffer->Release();
    }
}

void ParticleSystem::Update(ID3D11DeviceContext* context, float deltaTime) {

    context->CSSetShader(m_computeShader.Get(), nullptr, 0);
    
    context->CSSetShaderResources(1, 1, &resourceview);
    context->CSSetUnorderedAccessViews(0, 1, &uAccesView, nullptr);

    //Parameters
    context->CSSetShaderResources(0, 1, &NewSimulationStateSRV);   // register(u0)
    context->CSSetShaderResources(1, 1, &CurrentSimulationStateSRV); // register(u1)

    //--------------------Constant buffers----------------------------------
    DirectX::XMFLOAT4  time; time.x = deltaTime;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    SimulationParameters* particlePtr;
    context->Map(simulationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    particlePtr = (SimulationParameters*)mappedResource.pData;
    particlePtr->ConsumerLocation = startpoint;
    particlePtr->EmitterLocation = endpoint;
    particlePtr->TimeFactors = time;
    context->Unmap(simulationBuffer, 0);
    context->PSSetConstantBuffers(0, 1, &simulationBuffer);

    ParticleCount* countPtr;
    context->Map(particleCountBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    countPtr = (ParticleCount*)mappedResource.pData;
    countPtr->n = MAX_PARTICLES;
    context->Unmap(particleCountBuffer, 0);
    context->PSSetConstantBuffers(1, 1, &particleCountBuffer);

    //------------------------------------------------------------------------

    context->Dispatch(2, 1, 1); //Values from compute shader
}

void ParticleSystem::Render(ID3D11DeviceContext* context) {
    context->IASetInputLayout(m_layout);							//set the input layout for the shader to match out geometry
    context->VSSetShader(m_vertexShader.Get(), 0, 0);				//turn on vertex shader

    //------------------GEOMETRY SHADER-------------------------------
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* matrixPtr;
    context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    matrixPtr = (MatrixBufferType*)mappedResource.pData;
    matrixPtr->world = *world;
    matrixPtr->view = *view;
    matrixPtr->projection = *projection;
    context->Unmap(m_matrixBuffer, 0);
    context->PSSetConstantBuffers(0, 1, &m_matrixBuffer);

    DirectX::XMFLOAT4  time; time.x = 0;
    SimulationParameters* particlePtr;
    context->Map(simulationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    particlePtr = (SimulationParameters*)mappedResource.pData;
    particlePtr->ConsumerLocation = startpoint;
    particlePtr->EmitterLocation = endpoint;
    particlePtr->TimeFactors = time;
    context->Unmap(simulationBuffer, 0);
    context->PSSetConstantBuffers(0, 1, &simulationBuffer);

    context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
    //----------------------------------------------------

    context->PSSetShader(m_pixelShader.Get(), 0, 0);				//turn on pixel shader
    // Set the sampler state in the pixel shader.

    context->PSSetSamplers(0, 1, &m_wrapState);
    context->PSSetShaderResources(0, 1, &colorTexture);

}