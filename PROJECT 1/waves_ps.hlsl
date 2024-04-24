// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

Texture2D shaderTexture : register(t0);
Texture2D shaderNormalTexture : register(t1);
Texture2D shaderShadowTexture : register(t2);

SamplerState SampleType : register(s0);
SamplerState SampleTypeClamp : register(s1);

cbuffer LightBuffer : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float padding;
};

cbuffer CameraBuffer : register(b1)
{
    float3 camPos;
};

cbuffer FogBuffer : register(b2)
{
    float startFog;
    float endFog;
    float water;
};


struct InputType
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

float WaterStartAlpha = 4;
float WaterFinishAlpha = 20;

float4 main(InputType input) : SV_TARGET
{

    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;

	// Invert the light direction for calculations.
    lightDir = normalize(input.position3D - lightPosition);

    input.tex.y = 1 - input.tex.y;
    
    float3 normalMap;
    float3 normalSample = shaderNormalTexture.Sample(SampleType, input.tex);
    normalMap.x = normalSample.x * 2 - 1;
    normalMap.y = normalSample.y * 2 - 1;
    normalMap.z = normalSample.z * 2 - 1;
    float3x3 tbn = float3x3(normalize(input.tangent), normalize(input.bitangent), normalize(input.normal));
    normalMap = mul(normalMap, tbn);
    
	// Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(normalMap, -lightDir));

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
    color = saturate(color);


	
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    color = color * textureColor;
    
    float3 realPos;
    
    realPos.x = camPos.x - 40;
    realPos.y = camPos.y;
    realPos.z = camPos.z - 40;
    
    float3 diff = input.position3D - realPos;
    
    float distanceToCamera = sqrt(dot(diff, diff));
    
    color.a = 0.7;
    //float4 fogC;
    //if (water == 0)
    //    fogC = float4(0.5, 0.5, 0.5, 1);
    //else
    //    fogC = float4(0, 0, 0.7, 1);
    
    //if (distanceToCamera > startFog)
    //{
    //    float fogFactor = saturate((distanceToCamera - startFog) / (endFog - startFog));
    //    float4 finalColor = lerp(color, fogC, fogFactor);
        
    //    float finalAlpha = lerp(0.7, 1, fogFactor);
    //    finalColor.a = finalAlpha;
        
    //    color = finalColor;
    //}
    //else
    //{
    //    color.a = 0.7f;
    //}
    
    return color;

}