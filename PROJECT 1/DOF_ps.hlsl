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
	
    //DISTANCE VALUES
    float depth = depthTexture.Sample(SampleType, position).r;
    
    float depthThreshold = 0.9996;
    
    //BLUR VALUES
    float blurValue = 0.005;
    float4 blurredColor = float4(0.0, 0.0, 0.0, 0.0);
    float blurRange = 4; // 9 pixels -4 to 4
    float normalize = 0.055; // get the color I want)
    
    if (depth > depthThreshold)
    {
        for (int i = -blurRange; i <= blurRange; i++)
        {
            float2 offset = float2(i * blurValue, 0);
            blurredColor += shaderTexture.Sample(SampleType, position + offset) * normalize;
        }
    
        for (int i = -blurRange; i <= blurRange; i++)
        {
            float2 offset = float2(0, i * blurValue);
            blurredColor += shaderTexture.Sample(SampleType, position + offset) * normalize;
        }
        
        return blurredColor;
    }
    else
    {
        return textureColor;
    }
}