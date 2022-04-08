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
    return output;
}

DIFFUSEMAP

[[fn]]
float4 PS(VS_OUTPUT IN) : SV_Target0
{    
    return diffuseTex.Sample(ssLinear, IN.TC);
    //return heightTex.Sample(ssLinear, IN.TC);
    //return float4(IN.Pos.xyz, 1);
    //return float4(1, 0, 0, 1);

}

technique main
{
    pass p0
    {
        VertexShader = VS;
        PixelShader = PS;
    }
}




