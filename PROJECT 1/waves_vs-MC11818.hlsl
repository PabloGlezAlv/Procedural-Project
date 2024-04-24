cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
};

float PI = 3.141592;

cbuffer WavesBuffer : register(b1)
{
    float time;
    float amplitude;
    float frequency;
};

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
    float3 worldPos : TEXCOORD1;
    float3 normal : NORMAL;
    float3 position3D : TEXCOORD2;
    float3 positionWorld : TEXCOORD3;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
    float4 lightViewPosition : TEXCOORD4;
};

OutputType main(InputType input)
{
    //https://github.com/Imperat/DirectX-CourseWork-2016/blob/master/Shader.fx
    //
    
    OutputType output;
    output.positionWorld = mul(input.position, worldMatrix);
    
	// width and height of grid
    int u = 64;
    int v = 64;

    float s_X = 0;
    float s_Y = 0;
    float s_Z = 0;


    float2 X0 = float2(input.position.x, input.position.z);
    float height = 0;
    for (int i = 0;i < 3; i++) //Composite wave
    {
        float2 K = float2(1, i);
        float a = amplitude; // a is amplitude

        float2 X = X0 - K * a * sin(dot(K, X0) - frequency * time * 10);
        float y = a * cos(dot(K, X0) - frequency * time * 10);
        s_X += X[0] / u - 0.5f;
        s_Z += X[1] / v - 0.5f;
        s_Y += y;
    }
    
    input.position.x = s_X;
    input.position.z = s_Z;
    input.position.y = s_Y;

    input.position *= 7.5f;
    input.position.w /= 7.5f;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = input.tex *8;
    output.worldPos = output.position.xyz;
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.position3D = input.position.xyz;

    output.tangent = input.tangent;
    output.bitangent = input.bitangent;
    
    output.lightViewPosition = mul(output.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

   return output;
}