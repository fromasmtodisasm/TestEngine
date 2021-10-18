Language HLSL

HLSLShader
{
    
	//--------------------------------------------------------------------------------------
	// File: Tutorial02.fx
	//
	// Copyright (c) Microsoft Corporation. All rights reserved.
	//--------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------
	// Vertex Shader
	//--------------------------------------------------------------------------------------
	float4 VS( float3 Pos : POSITION ) : SV_POSITION
	{
		return float4(Pos, 1.0);
	}


	//--------------------------------------------------------------------------------------
	// Pixel Shader
	//--------------------------------------------------------------------------------------
	float4 PS( float4 Pos : SV_POSITION ) : SV_Target
	{
		return float4( 1.0f, 1.0f, 0.0f, 1.0f );    // Yellow, with Alpha = 1
	}
}

Technique main {
    Pass p0
    {
        VertexShader = VS
        PixelShader = PS
    }
}