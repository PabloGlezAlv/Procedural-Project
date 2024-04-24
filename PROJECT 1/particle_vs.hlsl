struct Particle
{
    float3 position;
    float3 direction;
    float time;
};

StructuredBuffer<Particle> SimulationState;

struct VS_INPUT
{
    uint vertexid : SV_VertexID;
};

struct GS_INPUT
{
    float3 position : Position;
};

GS_INPUT main(in VS_INPUT input)
{
    GS_INPUT output;
    
    output.position.xyz = SimulationState[input.vertexid].position;
    
    return output;
}