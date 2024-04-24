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
    //COLOR VALUES
    float2 position = float2(input.colour.x, input.colour.y);
    float4 textureColor = shaderTexture.Sample(SampleType, position);
	
    //return float4(position.x, position.y, 0, 1);
    float2 center = float2(0.5, 0.5);
    float2 diff = position - center;
    float distance =  length(diff);
    
    if (distance > 0.4)
    {
        if(distance < 0.5)
        {
            float lerp = (0.5 - distance) * 10;
            float colorBlack = 1 - lerp;
            
            return textureColor * lerp + float4(0, 0, 0, 1) * colorBlack;

        }
        else
        {
            return float4(0, 0, 0, 1);
        }
    }
    else 
        return textureColor;

}