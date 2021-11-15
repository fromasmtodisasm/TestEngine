#include "hlsl_common.fx"

// Shader global descriptions
float Script : STANDARDSGLOBAL
<
    string Script =
        // Determines if the shader will be made visible to UI elements
        // For example, if set the shader will be available in the Material Editor
        "Public;"
        // If set when SupportsFullDeferredShading is not set, we go through the tiled forward pass
        // In the tiled forward case, we will execute the pass defined in this file - and not Common*Pass
        "SupportsDeferredShading;"
        // Determines that the shader supports fully deferred shading, so will not go through the forward pass
        "SupportsFullDeferredShading;";
>;

//FIXME: shader parser
//StructuredBuffer<float4x4> Transform;


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TC : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TC : TEXCOORD0;
};

Texture2D g_FontAtlas : register(t0);
SamplerState g_LinearSampler : register(s0);

[[fn]]
VS_OUTPUT VS(
    VS_INPUT IN
)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Pos = Transform(IN.Pos);
    output.Normal = IN.Normal;
    output.TC = IN.TC;

    return output;
}

[[fn]]
float4 PS(VS_OUTPUT input)
	: SV_Target
{
    //FIXME: need recognize storage class in shader parser to place this declaration on top level
    float3 diffuseColor = float3(1, 1, 1);

    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = g_FontAtlas.Sample(g_LinearSampler, input.TC);

	// Invert the light direction for calculations.
    lightDir = SunDirection.xyz;

	// Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.Normal, lightDir));

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = float4(saturate(SunColor.rgb * diffuseColor * lightIntensity), 1);

	// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
    color = (color + AmbientStrength) * textureColor;

    return color;
}

//--------------------------------------------------------------------------------------
//technique10 Render
//{
//    pass P0
//    {
//        SetVertexShader(CompileShader(vs_4_0, VS()));
//        SetGeometryShader(NULL);
//        SetPixelShader(CompileShader(ps_4_0, PS()));
//    }
//}
technique Render
{
    pass P0
    {
        VertexShader = VS;
        PixelShader = PS;
    }
}

technique GrayScaleT
{
    pass P0
    {
        VertexShader = VS();
        PixelShader = GrayScale();
    }
}