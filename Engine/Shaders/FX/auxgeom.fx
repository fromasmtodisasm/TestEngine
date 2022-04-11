#include "Common.cfi"

Texture2D text : register(t0);
SamplerState textSampler : register(s0);

struct VsOutput
{
    float4 Position : SV_Position;
    float2 TexCoords : TEXCOORD;
    float4 Color : COLOR;
};

struct VsInput
{
    float3 Pos : POSITION;
    float4 Color : COLOR0;
    float2 TC : TEXCOORD0;
};

[[fn]]
VsOutput FontVS(VsInput IN)
{
    VsOutput Output;
    Output.Position = float4(IN.Pos.x,IN.Pos.y, 0.0, 1.0);
    Output.TexCoords = IN.TC;
    Output.Color = IN.Color;
    return Output;
}  

[[fn]]
float4 FontPS(VsOutput IN) : SV_Target0
{    
    // {{{{{{{{{{{{{{{{{
    return IN.Color * float4(1.0, 1.0, 1.0, text.Sample(textSampler, IN.TexCoords).r);
}

[[fn]]
// same function declaration style as vertex shaders
// pixel shaders return the colour value of the pixel (hence the float4)
float4 GrayScalePS(float3 color) : SV_Target0
{
  // greyscale the pixel colour values
  // - perform a dot product between the pixel colour and the specified vector
  // - 0.222, 0.707, 0.071 is found throughout image processing for gray scale effects.
    float4 grey = dot(float3(0.222, 0.707, 0.071), color);
  
  // return the pixel colour in the form of a float4.          
    return grey;
}

[[fn]]
float4 TexturedQuadPS(VsOutput IN) : SV_Target0
{    
    float4 color = text.Sample(textSampler, IN.TexCoords);
    return color;
}

Technique Font
<
    string description = "This is technique spesialized for font rendering";
>
{
    Pass p0<string script = "Decal";>
    {
        VertexShader = FontVS();
        PixelShader = FontPS();
    }
}

Technique TexturedQuad<string description = "This is technique spesialized for textured quad rendering";>
{
    Pass p0
    {
        VertexShader = FontVS();
        PixelShader = TexturedQuadPS();
    }
}

[[fn]]
VsOutput AuxVS(VsInput IN)
{
    VsOutput OUT = (VsOutput)0;
    OUT.Position = mul(GetViewProjMat(), float4(IN.Pos, 1.0));
    OUT.Color = float4(IN.Color);
    return OUT;
}
 
[[fn]]
float4 AuxPS(VsOutput IN) : SV_Target0
{
    return IN.Color;
}

// Default technique for auxiliary geometry rendering
technique AuxGeometry
{
    pass p0
    {
        VertexShader = AuxVS();
        PixelShader = AuxPS();
    }
}