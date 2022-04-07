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

[[fn]]
VS_OUTPUT VS(
    VS_INPUT IN
)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Pos = Transform(IN.Pos);
    //output.WorldPos = WorldTransofrm(IN.Pos).xyz;

    return output;
}

DIFFUSEMAP

[[fn]]
float4 PS(VS_OUTPUT IN) : SV_Target0
{    
    return diffuseTex.Sample(ssLinear, IN.TC);
}

technique main
{
    pass p0
    {
        VertexShader = VS;
        PixelShader = PS;
    }
}




