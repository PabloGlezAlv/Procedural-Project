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

cbuffer WavesBuffer : register(b3)
{
    float time;
    float amplitude;
    float frequency;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 position3D : TEXCOORD2;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
    float4 lightViewPosition : TEXCOORD3;
};

//Use the water code to determine if pixel need normal shadow or under water shadow (blue)
bool underWater(float myPosX, float myPosZ, float amplitude, float time, float frequency)
{
    // width and height of grid
    int u = 64;
    int v = 64;

    float s_X = 0;
    float s_Y = 0;
    float s_Z = 0;

    float4 finalPos = float4(1, 1, 1, 1);
    
    float2 X0 = float2(myPosX, myPosX);
    float height = 0;
    for (int i = 0; i < 3; i++)
    {
        float2 K = float2(1, i);
        float a = amplitude; // a is amplitude

        float2 X = X0 - K * a * sin(dot(K, X0) - frequency * time * 10);
        float y = a * cos(dot(K, X0) - frequency * time * 10);
        s_X += X[0] / u - 0.5f;
        s_Z += X[1] / v - 0.5f;
        s_Y += y;
    }
    
    finalPos.x = s_X;
    finalPos.z = s_Z;
    finalPos.y = s_Y;

    finalPos *= 7.5f;
    finalPos.w /= 7.5f;
    
    return finalPos.z < 0.1;
}


float4 main(InputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;

	// Invert the light direction for calculations.
    lightDir = normalize(input.position3D - lightPosition);

	// Calculate the amount of light on this pixel.
    // https://www.youtube.com/watch?v=Tf2ZOyc0XcQ&ab_channel=ChiliTomatoNoodle
    // https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
    
    input.tex.y = 1 - input.tex.y;
    
    float3 normalMap;
    float3 normalSample = shaderNormalTexture.Sample(SampleType, input.tex);
    normalMap.x = normalSample.x * 2 - 1;
    normalMap.y = normalSample.y * 2 - 1;
    normalMap.z = normalSample.z * 2 - 1;
    float3x3 tbn = float3x3(normalize(input.tangent), normalize(input.bitangent), normalize(input.normal));
    
    normalMap = mul(normalMap, tbn);
    
    lightIntensity = saturate(dot(normalMap, -lightDir));
    
	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
    color = saturate(color);

	
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    color = color * textureColor;
    
    float3 diff = input.position3D - camPos;
    
    float distanceToCamera = sqrt(dot(diff, diff));
    
 //   //------------------SHADOW--------------------------------
    
 //   //Copyright © 2012-2024 Matthias Moulin. All Rights Reserved.
    
 //   //Adaptation from: https://github.com/matt77hias/RasterTek/tree/master/Tutorial%2040_Shadow%20Mapping
    
 //   //__author__ = 'matthias.moulin@gmail.com (Matthias Moulin)'
    
 //   // Set the bias value for fixing the floating point precision issues.
 //   float bias = 0.00001f;

 //   //float4 light = float4(0, 0, 1, 1);
 //   //
 //   //if (underWater( input.position3D.x, input.position3D.z, amplitude, time, frequency))
 //   //{
 //   //    light = float4(1, 0, 0, 1);
 //   //}
    
	//// Set the default output color to the ambient light value for all pixels.
 //   float4 colorShadow = color/* * light*/;

	//// Calculate the projected texture coordinates.
 //   float2 projectTexCoord;
 //   projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
 //   projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	//// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
 //   if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y) && distanceToCamera < endFog)
 //   {
	//	// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
 //       float depthValue = shaderShadowTexture.Sample(SampleTypeClamp, projectTexCoord).r;

	//	// Calculate the depth of the light.
 //       float lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

	//	// Subtract the bias from the lightDepthValue.
 //       lightDepthValue = lightDepthValue - bias;

	//	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	//	// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
 //       if (lightDepthValue < depthValue)
 //       {
	//	    // Calculate the amount of light on this pixel.
 //           lightIntensity = saturate(dot(input.normal, lightPosition));

 //           if (lightIntensity > 0.0f)
 //           {
	//			// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
 //               colorShadow += (diffuseColor * lightIntensity);

	//			// Saturate the final light color.
 //               colorShadow = saturate(colorShadow);
 //           }
 //       }
        
        
 //       return color * colorShadow;
 //   }

    return color;
}

