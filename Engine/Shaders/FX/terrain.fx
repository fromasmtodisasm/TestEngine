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
    float2 TC : TEXCOORD0;
    float viewDistance : FOG;
};
HEIGHTMAP

[[fn]]
VS_OUTPUT VS(
    VS_INPUT IN
)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    float height = heightTex[IN.TC].r;
    height = heightTex.SampleLevel(ssLinear, IN.TC, 0).r;
    output.TC = IN.TC;

    float3 newPos = IN.Pos + float3(0, height - 0.5, 0);
    output.Pos = Transform(newPos);
    
    output.viewDistance = ModelViewTransofrm(IN.Pos).z;
    // Calculate linear fog.    
    return output;
}

DIFFUSEMAP

[[fn]]
float4 PS(VS_OUTPUT IN) : SV_Target0
{    
    float4 textureColor;
    float4 finalColor;

    // Sample the texture pixel at this location.
    textureColor = diffuseTex.Sample(ssLinear, IN.TC);
    
    // Calculate the final color using the fog effect equation.

    float fogFactor = saturate((fogEnd - IN.viewDistance) / (fogEnd - fogStart));
    finalColor = /*fogFactor * */textureColor; // + (1.0 - fogFactor) * float4(fogColor, 1);

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




