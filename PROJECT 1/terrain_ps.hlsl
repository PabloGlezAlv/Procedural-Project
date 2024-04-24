// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

Texture2D sandTexture : register(t0);
Texture2D sandNormalTexture : register(t1);
Texture2D shaderShadowTexture : register(t2);
Texture2D snowTexture : register(t3);
Texture2D snowNormalTexture : register(t4);
Texture2D grassTexture : register(t5);
Texture2D grassNormalTexture : register(t6);
Texture2D rockTexture : register(t7);
Texture2D rockNormalTexture : register(t8);

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
    float3 normal : NORMAL;
    float3 position3D : TEXCOORD2;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
    float4 lightViewPosition : TEXCOORD3;
};

float4 main(InputType input) : SV_TARGET
{
	float4	textureColor;
    float3	lightDir;
    float	lightIntensity;
    float4	color;
    
	// Invert the light direction for calculations.
	lightDir = normalize(input.position3D - lightPosition);

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, -lightDir));

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
	color = saturate(color);
    
    float slope = 1 - input.normal.y;
    
    float4 realColor;
    
    if (slope > 0.4 && input.position3D.y > 2)
    {
        realColor = rockTexture.Sample(SampleType, input.tex * 10);   
    }
	else if(input.position3D.y > 5) //---------------SNOW-----------------------
    {
        realColor = snowTexture.Sample(SampleType, input.tex * 10);
        if (input.position3D.y < 7)
        {
            float floatWhiteValue = (input.position3D.y - 5) / 2;
            float floatGreenValue = 1 - floatWhiteValue;

            realColor = (snowTexture.Sample(SampleType, input.tex * 10) * floatWhiteValue) + 
            (grassTexture.Sample(SampleType, input.tex * 10) * floatGreenValue);

        }
        
        if (slope > 0.3)
        {
            float floatNormalValue = (0.4 - slope) / 0.1;
            float floatRockValue = 1 - floatNormalValue;
            
            realColor = rockTexture.Sample(SampleType, input.tex * 10) * floatRockValue + realColor * floatNormalValue;

        }
         
    }
    else if (input.position3D.y > 0.5) //---------------GRASS-----------------------
    {
        realColor = grassTexture.Sample(SampleType, input.tex * 10);
        if (input.position3D.y < 1.5)
        {
            float floatWhiteValue = (input.position3D.y - 0.5);
            float floatGreenValue = 1 - floatWhiteValue;

            realColor = (grassTexture.Sample(SampleType, input.tex * 10) * floatWhiteValue) +
            (sandTexture.Sample(SampleType, input.tex * 10) * floatGreenValue);

        }
        
                
        if (slope > 0.3)
        {
            float floatNormalValue = (0.4 - slope) / 0.1;
            float floatRockValue = 1 - floatNormalValue;
            
            realColor = rockTexture.Sample(SampleType, input.tex * 10) * floatRockValue + realColor * floatNormalValue;

        }
    }
    else if (input.position3D.y < 0.5) //---------------SAND-----------------------
    {
        realColor = sandTexture.Sample(SampleType, input.tex * 10);
    }
	
	color = color * realColor;

    //---------------------------FOG----------------------------------
    
    float3 diff = input.position3D - camPos;
    
    float distanceToCamera = sqrt(dot(diff, diff));
    
    float4 fogC;
    //if (water == 0)
    //    fogC = float4(0.5, 0.5, 0.5, 1);
    //else
    //    fogC = float4(0, 0, 0.7, 1);
    
    //if (distanceToCamera > startFog)
    //{
    //    float fogFactor = saturate((distanceToCamera - startFog) / (endFog - startFog));
    //    float4 finalColor = lerp(color, fogC, fogFactor);
    //    color = finalColor;
    //}
    
    return color;
    
 //   //------------------SHADOW--------------------------------
    
 //   //Copyright © 2012-2024 Matthias Moulin. All Rights Reserved.
    
 //   //Adaptation from: https://github.com/matt77hias/RasterTek/tree/master/Tutorial%2040_Shadow%20Mapping
    
 //   //__author__ = 'matthias.moulin@gmail.com (Matthias Moulin)'
    
 //   // Set the bias value for fixing the floating point precision issues.
 //   float bias = 0.000001f;

 //   //float4 light = float4(0, 0, 1, 1);
 //   //
 //   //if (underWater( input.position3D.x, input.position3D.z, amplitude, time, frequency))
 //   //{
 //   //    light = float4(1, 0, 0, 1);
 //   //}
    
	//// Set the default output color to the ambient light value for all pixels.
 //   float4 colorShadow = color /* * light*/;

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
 //   }

 //   //return colorShadow;
    
 //   return colorShadow;
}

