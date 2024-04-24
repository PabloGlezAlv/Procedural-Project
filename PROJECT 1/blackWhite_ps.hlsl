// Colour pixel/fragment shader
// Basic fragment shader outputting a colour

Texture2D shaderTexture : register(t0);
Texture2D depthTexture : register(t1);
SamplerState SampleType : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
};


float4 main(InputType input) : SV_TARGET
{
    float2 position = float2(input.colour.x, input.colour.y);
    
    float4 textureColor = shaderTexture.Sample(SampleType, position);
	
    float4 outputColor;
    float grayscale = (textureColor.x + textureColor.y + textureColor.z) / 3;
    
    return float4(grayscale, grayscale, grayscale, 1);
	
    return outputColor;
}