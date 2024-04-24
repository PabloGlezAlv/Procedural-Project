#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "Shader.h"

#define MAX_PARTICLES 1023

class ParticleSystem {
public:
    ParticleSystem(ID3D11Device* device, int maxParticles, DirectX::XMMATRIX* world,
        DirectX::XMMATRIX* view, DirectX::XMMATRIX* projection);
    ~ParticleSystem();

    void Update(ID3D11DeviceContext* context, float deltaTime);
    void Render(ID3D11DeviceContext* deviceContext);

private:
    struct Particle {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 velocity;
        float lifetime;
    };

    struct MatrixBufferType
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

    struct SimulationParameters
    {
        DirectX::XMFLOAT4  EmitterLocation;
        DirectX::XMFLOAT4  ConsumerLocation;
        DirectX::XMFLOAT4  TimeFactors;
    };

    struct ParticleCount
    {
        int n;
    };

    DirectX::XMFLOAT4 startpoint;
    DirectX::XMFLOAT4 endpoint;

    ID3D11Buffer* m_matrixBuffer;
    ID3D11Buffer* particleBuffer;
    ID3D11Buffer* particleCountBuffer;
    ID3D11Buffer* simulationBuffer;

    ID3D11Buffer* newSimulationBuffer;
    ID3D11Buffer* currentSimulationBuffer;

    float m_particleLifetime; // Tiempo de vida inicial de las partículas

    Microsoft::WRL::ComPtr<ID3D11VertexShader>								m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>								m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>								m_computeShader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>							m_geometryShader;

    ID3D11InputLayout* m_layout;

    ID3D11SamplerState* m_wrapState;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> colorTexture;

    ID3D11ShaderResourceView* resourceview;
    ID3D11UnorderedAccessView* uAccesView;


    ID3D11ShaderResourceView* CurrentSimulationStateSRV;
    ID3D11ShaderResourceView* NewSimulationStateSRV;

    DirectX::XMMATRIX* world;
    DirectX::XMMATRIX* view;
    DirectX::XMMATRIX* projection;
};