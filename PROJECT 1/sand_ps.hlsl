// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

Texture2D shaderTexture : register(t0);
Texture2D shaderNormalTexture : register(t1);
Texture2D shaderShadowTexture : register(t2);
Texture2D shader2Texture : register(t3);
Texture2D shaderNormal2Texture : register(t4);

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
    bool water;
};

struct InputType
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

float4 main(InputType input) : SV_TARGET
{
    float4 textureColor, textureColorWet;
    float3 lightDir;
    float lightIntensity;
    float4 color, colorWet;


    // Invert the light direction for calculations.
    lightDir = normalize(input.position3D - lightPosition);

    // Calculate the amount of light on this pixel.

    input.tex.y = 1 - input.tex.y;

    float3 normalMap;
    float3 normalSample = shaderNormalTexture.Sample(SampleType, input.tex);
    normalMap.x = normalSample.x * 2 - 1;
    normalMap.y = normalSample.y * 2 - 1;
    normalMap.z = normalSample.z * 2 - 1;
    float3x3 tbn = float3x3(normalize(input.tangent), normalize(input.bitangent), normalize(input.normal));

    //return float4(normalSample.x, normalSample.y, normalSample.z, 1);

    normalMap = mul(normalMap, tbn);

    lightIntensity = saturate(dot(normalMap, -lightDir));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
    color = saturate(color);

    
    //WetTexture 
    float3 normalMapWet;
    float3 normalSampleWet = shaderNormal2Texture.Sample(SampleType, input.tex);
    normalMapWet.x = normalSampleWet.x * 2 - 1;
    normalMapWet.y = normalSampleWet.y * 2 - 1;
    normalMapWet.z = normalSampleWet.z * 2 - 1;
    float3x3 tbnWet = float3x3(normalize(input.tangent), normalize(input.bitangent), normalize(input.normal));

    normalMap = mul(normalMapWet, tbnWet);

    lightIntensity = saturate(dot(normalMapWet, -lightDir));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    colorWet = ambientColor + (diffuseColor * lightIntensity); //adding ambient
    colorWet = saturate(colorWet);
    

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    textureColorWet = shader2Texture.Sample(SampleType, input.tex);

    float startLerp = 4.5;
    float finishLerp = 7;
    
    if (input.worldPos.x > finishLerp)
    {
        color = color * textureColor;
    }
    else /*if (input.worldPos.x < startLerp)*/
    {
        color = color * textureColorWet;
    }
    

    float3 diff = input.position3D - camPos;
    
    float distanceToCamera = sqrt(dot(diff, diff));
    
    float4 fogC;
    if (water == 0)
        fogC = float4(0.5, 0.5, 0.5, 1);
    else
        fogC = float4(0, 0, 0.7, 1);
    
    if (distanceToCamera > startFog)
    {
        float fogFactor = saturate((distanceToCamera - startFog) / (endFog - startFog));
        float4 finalColor = lerp(color, fogC, fogFactor);
        color = finalColor;
    }
    
    //------------------SHADOW--------------------------------
    // Set the bias value for fixing the floating point precision issues.
    float bias = 0.000001f;

	// Set the default output color to the ambient light value for all pixels.
    float4 colorShadow = color;

	// Calculate the projected texture coordinates.
    float2 projectTexCoord;
    projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
        float depthValue = shaderShadowTexture.Sample(SampleTypeClamp, projectTexCoord).r;

		// Calculate the depth of the light.
        float lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		// Subtract the bias from the lightDepthValue.
        lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
        if (lightDepthValue < depthValue)
        {
		    // Calculate the amount of light on this pixel.
            lightIntensity = saturate(dot(input.normal, lightPosition));

            if (lightIntensity > 0.0f)
            {
                colorShadow += (diffuseColor * lightIntensity);

                // Saturate the final light color.
                colorShadow = saturate(colorShadow);
            }
        }
    }

    return color * colorShadow;
}

