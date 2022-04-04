
//=== Base stream definitions ==================================================

#define IN_P    float4 Position  : POSITION;
#define IN_C0   float4 Color     : COLOR;
#define IN_TBASE float2 baseTC   : TEXCOORD0; 

// ==== Additional streams ==================================

#define IN_TANG_STREAM                                                               \
          float4 Tangent  : TANGENT;                                                 \
          float4 Binormal : BITANGENT;                                               \

#define OUT_C0   half4 Color     : SV_Target0;
#define OUT_C1   half4 Color1    : SV_Target1;

//=== Pixel shader output structure ==========================================

struct pixout
{
  half4 Color  : SV_Target0;
};

struct pixout_fp
{
  float4 Color  : SV_Target0;
};

struct pixout_gbuffer
{
	half4 GBufferA : SV_Target0;
	
#if !_RT_SAMPLE0
	half4 GBufferB : SV_Target1;
	half4 GBufferC : SV_Target2;
	
#if _RT_MOTION_BLUR
	half2 VelocityRT  : SV_Target3;
#endif

#if (_RT_MSAA_QUALITY || _RT_MSAA_QUALITY1) && _RT_ALPHATEST
	uint uCoverageMask	: SV_Coverage;
#endif

#if SILHOUETTE_PARALLAX_OCCLUSION_MAPPING
	#if !_RT_DISSOLVE && !TEMP_VEGETATION && !TEMP_TERRAIN
		#if !_RT_REVERSE_DEPTH
			float fDepth : SV_DepthGreaterEqual;
		#else
			float fDepth : SV_DepthLessEqual;
		#endif
	#endif
#endif
#endif
};
