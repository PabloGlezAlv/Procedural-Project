// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader

Texture2D shaderHeightTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
    bool heightMap;
};

float flatSandX = 9;

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 position3D : TEXCOORD2;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
    float4 worldPos : TEXCOORD3;
    float4 lightViewPosition : TEXCOORD4;
};

OutputType main(InputType input)
{
    OutputType output;

    float4 height = shaderHeightTexture.SampleLevel(SampleType, input.tex * 3, 0);

    input.position.w = 1.0f;
    
    float4 worldPosition = mul(input.position, worldMatrix);
    float4 viewPosition = mul(worldPosition, viewMatrix);
    float4 projectedPosition = mul(viewPosition, projectionMatrix);
    
    output.worldPos = worldPosition;
    
    //if (worldPosition.x > flatSandX)
    //{
        output.position = projectedPosition;
    if (heightMap)
        output.position.y += height.y;
    //}
    //else
    //{
    //    output.position = projectedPosition;
    //    float lerp = worldPosition.x / flatSandX;
    //    if (lerp < 0.1f)
    //        lerp = 0.1f;
    //    output.position.y += height.y * lerp;
    //}
    
    output.tex = input.tex * 3;
    
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    
    output.normal = normalize(output.normal);
    
    output.position3D = (float3) worldPosition;

    output.tangent = input.tangent;
    output.bitangent = input.bitangent;

    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);
    
    return output;
}