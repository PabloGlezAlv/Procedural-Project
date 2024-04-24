Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texcoords : TEXCOORD0;
    float4 color : TEXCOORD1;
};

float4 main(in PS_INPUT input) : SV_TARGET
{
    float4 color = shaderTexture.Sample(SampleType, input.texcoords);
    
    color = color * input.color;
    
    return color;
    return float4(1.0f, 1.0f, 0.0f, 1.0f); 
}