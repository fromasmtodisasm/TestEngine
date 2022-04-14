#include "common.cfi"

#include "shadeLib.cfi"
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 TC : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 W : WORLD_POSITION;
    float3 N : NORMAL;
    float2 TC : TEXCOORD0;
    float viewDistance : FOG;
};
HEIGHTMAP

float height(float2 pos)
{
    return heightTex.SampleLevel(ssLinear, pos, 0).r;
}

[fn]
VS_OUTPUT VS(
    VS_INPUT IN
)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    // # P.xy store the position for which we want to calculate the normals
  // # height() here is a function that return the height at a point in the terrain

  // read neightbor heights using an arbitrary small offset
    float3 off = float3(1.0 / 65, 1.0 / 65, 0.0);
    float hL = height(IN.Pos.xz - off.xz);
    float hR = height(IN.Pos.xz + off.xz);
    float hD = height(IN.Pos.xz - off.zy);
    float hU = height(IN.Pos.xz + off.zy);

  // deduce terrain normal
    float3 N;
    N.x = hL - hR;
    N.y = hD - hU;
    N.z = 2.0;
    N = normalize(N);

    output.TC = IN.TC;
    output.N = WorldTransofrm(N);

    float h = height(IN.Pos.xz);
    float3 newPos = IN.Pos + float3(0, h, 0);
    output.Pos = Transform(newPos);
    output.W = WorldTransofrm(newPos);
    
    output.viewDistance = ModelViewTransofrm(IN.Pos).z;
    // Calculate linear fog.    
    return output;
}

DIFFUSEMAP
//SunDirection
[fn]
float4 PS(VS_OUTPUT IN) : SV_Target0
{    
    float4 textureColor;
    float4 finalColor;

    // Sample the texture pixel at this location.
    textureColor = diffuseTex.Sample(ssLinear, IN.TC);
    
    // Calculate the final color using the fog effect equation.

    float fogFactor = saturate((fogEnd - IN.viewDistance) / (fogEnd - fogStart));
    finalColor = /*fogFactor * */textureColor; // + (1.0 - fogFactor) * float4(fogColor, 1);

    // Get light direction for this fragment
    //float3 lightDir = normalize(float3(10,120, 10) - IN.W);
    float3 lightDir = normalize(float3(0, -1, 0));
    //float3 lightDir = normalize(SunDirection);

    // Note: Non-uniform scaling not supported
    float diffuseLighting = saturate(dot(IN.N, -lightDir)); // per pixel diffuse lighting

    // Introduce fall-off of light intensity
    //diffuseLighting *= ((length(lightDir) * length(lightDir)) / dot(light.Position - Input.WorldPosition, light.Position - Input.WorldPosition));


    //finalColor = float4(IN.N, 1);
    finalColor.rgb *= diffuseLighting.xxx + 0.3;
    //finalColor = lerp(finalColor, float4(1, 0, 0, 1), normalize(float4(IN.W, 1)));

    return finalColor;

}

technique main
{
    pass p0
    {
        VertexShader = VS;
        PixelShader = PS;
    }
}




