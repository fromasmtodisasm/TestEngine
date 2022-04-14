#pragma once

//////////////////////////////////////////////////////////////////////
class CRenderer;
extern CRenderer* gRenDev;

class CShader;

class CGlobalResources
{
public:
	~CGlobalResources();
	bool                     Valid();
	static CGlobalResources& Get()
	{
		static CGlobalResources s_Resources;
		return s_Resources;
	}

public:
	ComPtr<ID3D11SamplerState> LinearSampler;
	_smart_ptr<CShader>        TexturedQuadShader;
	ComPtr<ID3D11BlendState>   FontBlendState;

private:
	bool m_Valid = false;
	bool Init();
	CGlobalResources();
};

namespace DeviceFormats
{
	//---------------------------------------------------------------------------------------------------------------------
	inline UINT GetStride(D3DFormat format)
	{
		switch (format)
		{
		default:
		case DXGI_FORMAT_UNKNOWN:
			return 0;
			break;

		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return 4 * sizeof(float);
			break;

		case DXGI_FORMAT_R32G32B32_TYPELESS:
		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			return 3 * sizeof(float);
			break;

		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
			return 4 * sizeof(short);
			break;

		case DXGI_FORMAT_R32G32_TYPELESS:
		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
			return 2 * sizeof(float);
			break;

		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return 2 * sizeof(float);
			break;

		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R11G11B10_FLOAT:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
			return 4 * sizeof(char);
			break;

		case DXGI_FORMAT_R16G16_TYPELESS:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_UINT:
		case DXGI_FORMAT_R16G16_SNORM:
		case DXGI_FORMAT_R16G16_SINT:
			return 2 * sizeof(short);
			break;

		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
			return 1 * sizeof(float);
			break;

#if CRY_RENDERER_VULKAN
		case DXGI_FORMAT_D16_UNORM_S8_UINT:
		case DXGI_FORMAT_R16G8X8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM:
		case DXGI_FORMAT_R24X8_TYPELESS:
#endif
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return 4 * sizeof(char);
			break;

		case DXGI_FORMAT_R8G8_TYPELESS:
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_UINT:
		case DXGI_FORMAT_R8G8_SNORM:
		case DXGI_FORMAT_R8G8_SINT:
			return 2 * sizeof(char);
			break;

		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SNORM:
		case DXGI_FORMAT_R16_SINT:
			return 1 * sizeof(short);
			break;

		case DXGI_FORMAT_B5G6R5_UNORM:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
			return 1 * sizeof(short);
			break;

		case DXGI_FORMAT_R8_TYPELESS:
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
#if CRY_RENDERER_VULKAN
		case DXGI_FORMAT_S8_UINT:
#endif
		case DXGI_FORMAT_A8_UNORM:
			return 1 * sizeof(char);
			break;

		case DXGI_FORMAT_R1_UNORM:
			return 1 * sizeof(char);
			break;
		}

		CRY_ASSERT(0);
		return 0;
	}

} // namespace DeviceFormats

//////////////////////////////////////////////////////////////////////
// Resource conventions

enum EHWShaderClass : uint8
{
	eHWSC_Vertex   = 0,
	eHWSC_Pixel    = 1,
	eHWSC_Geometry = 2,
	eHWSC_Domain   = 3,
	eHWSC_Hull     = 4,
	eHWSC_NumGfx   = 5,

	eHWSC_Compute  = 5,
	eHWSC_Num      = 6
};

enum EShaderStage : uint8
{
	EShaderStage_Vertex            = BIT(eHWSC_Vertex),
	EShaderStage_Pixel             = BIT(eHWSC_Pixel),
	EShaderStage_Geometry          = BIT(eHWSC_Geometry),
	EShaderStage_Domain            = BIT(eHWSC_Domain),
	EShaderStage_Hull              = BIT(eHWSC_Hull),
	EShaderStage_Compute           = BIT(eHWSC_Compute),

	EShaderStage_CountGfx          = eHWSC_NumGfx,
	EShaderStage_Count             = eHWSC_Num,
	EShaderStage_None              = 0,
	EShaderStage_All               = EShaderStage_Vertex | EShaderStage_Pixel | EShaderStage_Geometry | EShaderStage_Domain | EShaderStage_Hull | EShaderStage_Compute,
	EShaderStage_AllWithoutCompute = EShaderStage_Vertex | EShaderStage_Pixel | EShaderStage_Geometry | EShaderStage_Domain | EShaderStage_Hull
};
DEFINE_ENUM_FLAG_OPERATORS(EShaderStage)
#define SHADERSTAGE_FROM_SHADERCLASS(SHADERCLASS) ::EShaderStage(BIT(SHADERCLASS))
#define SHADERSTAGE_FROM_SHADERCLASS_CONDITIONAL(SHADERCLASS, SET) ::EShaderStage((SET) << (SHADERCLASS))

enum EConstantBufferShaderSlot
{
	// Z/G/S-Buffer, Forward
	eConstantBufferShaderSlot_PerDraw                   = 0, // EShaderStage_All
	eConstantBufferShaderSlot_PerMaterial               = 1, // EShaderStage_All
	eConstantBufferShaderSlot_SkinQuat                  = 2, // EShaderStage_Vertex
	eConstantBufferShaderSlot_SkinQuatPrev              = 3, // EShaderStage_Vertex
	eConstantBufferShaderSlot_PerGroup                  = 4, // EShaderStage_Vertex | EShaderStage_Hull
	eConstantBufferShaderSlot_PerPass                   = 5, // EShaderStage_All
	eConstantBufferShaderSlot_PerView                   = 6, // EShaderStage_All
	eConstantBufferShaderSlot_VrProjection              = 7,
	
	// Scaleform
	eConstantBufferShaderSlot_ScaleformMeshAttributes   = 0, // EShaderStage_Vertex
	eConstantBufferShaderSlot_ScaleformRenderParameters = 0, // EShaderStage_Pixel

	// Primitive/Custom/Post
	eConstantBufferShaderSlot_PerPrimitive              = eConstantBufferShaderSlot_PerDraw,

	eConstantBufferShaderSlot_Max                       = 7,
	eConstantBufferShaderSlot_Count                     = 8,
};

enum EShaderResourceShaderSlot
{
};

enum EResourceLayoutSlot
{
	EResourceLayoutSlot_PerDrawCB                       = 0, // EShaderStage_Vertex | EShaderStage_Pixel | EShaderStage_Domain
	EResourceLayoutSlot_PerDrawExtraRS                  = 1,
	EResourceLayoutSlot_PerMaterialRS                   = 2,
	EResourceLayoutSlot_PerPassRS                       = 3,
	EResourceLayoutSlot_VrProjectionCB                  = 4,

	EResourceLayoutSlot_Max                             = 7,
	EResourceLayoutSlot_Num                             = 8
};

enum EReservedTextureSlot
{
	// Z/G/S-Buffer
	EReservedTextureSlot_SkinExtraWeights               = 14, // EShaderStage_Vertex (mutually exclusive with ComputeSkinVerts)
	EReservedTextureSlot_ComputeSkinVerts               = 14, // EShaderStage_Vertex (mutually exclusive with SkinExtraWeights)
	EReservedTextureSlot_GpuParticleStream              = 14, // EShaderStage_Vertex

	EReservedTextureSlot_DrawInstancingData             = 15, // EShaderStage_Vertex | EShaderStage_Pixel
	EReservedTextureSlot_AdjacencyInfo                  = 16, // EShaderStage_Domain
	EReservedTextureSlot_TerrainBaseMap                 = 29, // EShaderStage_Pixel (set where?)
	
	// Forward
	EReservedTextureSlot_LightvolumeInfos               = 33,
	EReservedTextureSlot_LightVolumeRanges              = 34,

	// Custom/Post
	EReservedTextureSlot_ParticlePositionStream         = 35,
	EReservedTextureSlot_ParticleAxesStream             = 36,
	EReservedTextureSlot_ParticleColorSTStream          = 37,
};

////////////////////////////////////////////////////////////////////////////
// ResourceView API

struct ResourceViewHandle
{
	typedef uint8 ValueType;
	ValueType value;

	constexpr ResourceViewHandle() : value(Unspecified) { }
	constexpr ResourceViewHandle(ValueType v) : value(v) { }

	// Test operators
	template<typename T> bool operator ==(const T other) const { return value == other; }
	template<typename T> bool operator !=(const T other) const { return value != other; }
	// Range operators
	template<typename T> bool operator <=(const T other) const { return value <= other; }
	template<typename T> bool operator >=(const T other) const { return value >= other; }
	// Sorting operators
	template<typename T> bool operator < (const T other) const { return value <  other; }
	template<typename T> bool operator > (const T other) const { return value >  other; }

	// Auto cast for array access operator []
	operator ValueType() const { return value; }

	enum PreDefs : ValueType
	{
		Unspecified = ValueType(~0),
	};
};
static_assert(sizeof(ResourceViewHandle::ValueType) == sizeof(ResourceViewHandle), "ResourceViewHandle is suppose to be as small as the base type");

struct SResourceView
{
	typedef uint64 KeyType;

	enum ResourceViewType
	{
		eShaderResourceView = 0,
		eRenderTargetView,
		eDepthStencilView,
		eUnorderedAccessView,

		eNumResourceViews
	};

	enum ResourceViewFlags
	{
		eSRV_DepthOnly   = 0,
		eSRV_StencilOnly = 1,

		eDSV_ReadWrite   = 0,
		eDSV_ReadOnly    = 1,

		eUAV_WriteOnly   = 0,
		eUAV_ReadWrite   = 1,
	};

	union ResourceViewDesc
	{
		// Texture View configuration
		struct
		{
			// 12 bits CommonHeader
			uint64 eViewType        : 3;
			uint64 nFormat          : 7;
			uint64 nFlags           : 2;

			uint64 bSrgbRead        : 1; // TODO: convert to flags sRGBRead | sRGBWrite for supporting sRGB render-target write
			uint64 bMultisample     : 1;

			uint64 nFirstSlice      : 11;
			uint64 nSliceCount      : 11;
			uint64 nMostDetailedMip : 4;
			uint64 nMipCount        : 4;
			
			uint64 nUnused          : 20;
		};

		// Buffer View configuration
		struct
		{
			uint64 nCommonHeader    : 12;

			uint64 bRaw             : 1;

			uint64 nOffsetBits      : 5;  // up to 2^2^5 -> 2^32 bits for the offset
			uint64 nOffsetAndSize   : 46; // N bits offset, then m bits size, both are given as element-addresses
		};

		KeyType Key;
	};

	SResourceView(uint64 nKey = 0)
	{
		static_assert(sizeof(m_Desc) <= sizeof(KeyType),"SResourceView: sizeof(m_Desc) <= sizeof(KeyType)");

		m_Desc.Key = nKey;
	}

	static SResourceView ShaderResourceView (DXGI_FORMAT nFormat, int nFirstSlice = 0, int nSliceCount = -1, int nMostDetailedMip = 0, int nMipCount = -1, bool bSrgbRead = false, bool bMultisample = false, int nFlags = 0);
	static SResourceView RenderTargetView   (DXGI_FORMAT nFormat, int nFirstSlice = 0, int nSliceCount = -1, int nMipLevel = 0, bool bMultisample = false);
	static SResourceView DepthStencilView   (DXGI_FORMAT nFormat, int nFirstSlice = 0, int nSliceCount = -1, int nMipLevel = 0, bool bMultisample = false, int nFlags = 0);
	static SResourceView UnorderedAccessView(DXGI_FORMAT nFormat, int nFirstSlice = 0, int nSliceCount = -1, int nMipLevel = 0, int nFlags = 0);
	
	static SResourceView ShaderResourceRawView (DXGI_FORMAT nFormat, int nFirstElement = 0, int nElementCount = -1, int nFlags = 0);
	static SResourceView RenderTargetRawView   (DXGI_FORMAT nFormat, int nFirstElement = 0, int nElementCount = -1);
	static SResourceView DepthStencilRawView   (DXGI_FORMAT nFormat, int nFirstElement = 0, int nElementCount = -1, int nFlags = 0);
	static SResourceView UnorderedAccessRawView(DXGI_FORMAT nFormat, int nFirstElement = 0, int nElementCount = -1, int nFlags = 0);

	static bool          IsMultisampled       (KeyType key) { return SResourceView(key).m_Desc.bMultisample; }
	static bool          IsShaderResourceView (KeyType key) { return SResourceView(key).m_Desc.eViewType == eShaderResourceView; }
	static bool          IsRenderTargetView   (KeyType key) { return SResourceView(key).m_Desc.eViewType == eRenderTargetView; }
	static bool          IsDepthStencilView   (KeyType key) { return SResourceView(key).m_Desc.eViewType == eDepthStencilView; }
	static bool          IsUnorderedAccessView(KeyType key) { return SResourceView(key).m_Desc.eViewType == eUnorderedAccessView; }

	bool                 operator ==(const SResourceView& other) const { return m_Desc.Key == other.m_Desc.Key; }
	bool                 operator !=(const SResourceView& other) const { return m_Desc.Key != other.m_Desc.Key; }

	ResourceViewDesc m_Desc;
};
static_assert(sizeof(SResourceView::KeyType) == sizeof(SResourceView), "SResourceView is suppose to be as small as the base type");

// TODO: Move to DeviceObjects.h
struct EDefaultResourceViews : ResourceViewHandle
{
	enum PreDefs : ValueType
	{
		Default            = 0,
		Alternative        = 1,

		RasterizerTarget   = 2,
		UnorderedAccess    = 3,

		PreAllocated       = 4, // from this value and up custom resource views are assigned

		// aliases for RGB data
		Linear             = Default,
		sRGB               = Alternative,

		// aliases for DepthStencil data
		DepthOnly          = Default,
		StencilOnly        = Alternative,

		// aliases for RasterizerTarget data
		RenderTarget       = RasterizerTarget,
		DepthStencil       = RasterizerTarget
	};
};
static_assert(sizeof(EDefaultResourceViews::ValueType) == sizeof(EDefaultResourceViews), "EDefaultResourceViews is suppose to be as small as the base type");

////////////////////////////////////////////////////////////////////////////
// SamplerState API (pre-allocated sampler states)

enum ESamplerReduction
{
	eSamplerReduction_Standard   = 0,
	eSamplerReduction_Comparison = 1,
	eSamplerReduction_Minimum    = 2,
	eSamplerReduction_Maximum    = 3,
};

enum ESamplerFilter
{
	eSamplerFilter_Point  = 0,
	eSamplerFilter_Linear = 1,
};

enum ESamplerAddressMode
{
	eSamplerAddressMode_Wrap   = 0,
	eSamplerAddressMode_Mirror = 1,
	eSamplerAddressMode_Clamp  = 2,
	eSamplerAddressMode_Border = 3
};

/* Not yet used
enum ESamplerMaxAnisotropy
{
	eSamplerMaxAniso_1x  = 0, // equates "off"
	eSamplerMaxAniso_2x  = 1,
	eSamplerMaxAniso_3x  = 2,
	eSamplerMaxAniso_4x  = 3,
	eSamplerMaxAniso_6x  = 4,
	eSamplerMaxAniso_8x  = 5,
	eSamplerMaxAniso_12x = 6,
	eSamplerMaxAniso_16x = 7,
};
*/

struct SSamplerState
{
	// TODO: optimize size and squeeze everything in one 64bit word
	struct
	{
		//	signed char m_nReduction  : 2; // 2 bit, ESamplerReduction

		signed char m_nMinFilter  : 8; // Currently: FILTER_XXX [-1,7] / TODO: Make 1 bit, ESamplerFilter
		signed char m_nMagFilter  : 8; // Currently: FILTER_XXX [-1,7] / TODO: Make 1 bit, ESamplerFilter
		signed char m_nMipFilter  : 8; // Currently: FILTER_XXX [-1,7] / TODO: Make 1 bit, ESamplerFilter

		signed char m_nAddressU   : 8; // Currently: ESamplerAddressMode / TODO: Make 2 bit
		signed char m_nAddressV   : 8; // Currently: ESamplerAddressMode / TODO: Make 2 bit
		signed char m_nAddressW   : 8; // Currently: ESamplerAddressMode / TODO: Make 2 bit

		signed char m_nAnisotropy : 8; // Currently: [0,16] / TODO: Make 3 bit, ESamplerMaxAnisotropy, (drop non-power-two values?)

		signed char m_nPadding    : 8;
	};

	DWORD m_dwBorderColor;                // TODO: 2 bit (index into palette [black, white, transp])
	float m_fMipLodBias;
	bool  m_bActive;                      // TODO: remove
	bool  m_bComparison;                  // TODO: deprecate, use m_nReduction, add comparison function
	bool  m_bSRGBLookup;                  // TODO: deprecate, use format
	byte  m_bPAD;                         // TODO: remove

	constexpr SSamplerState()
		: m_nMinFilter(FILTER_POINT)
		, m_nMagFilter(FILTER_POINT)
		, m_nMipFilter(FILTER_POINT)
		, m_nAddressU(eSamplerAddressMode_Wrap)
		, m_nAddressV(eSamplerAddressMode_Wrap)
		, m_nAddressW(eSamplerAddressMode_Wrap)
		, m_nAnisotropy(0)
		, m_nPadding(0)
		, m_dwBorderColor(0)
		, m_fMipLodBias(0.0f)
		, m_bActive(false)
		, m_bComparison(false)
		, m_bSRGBLookup(false)
		, m_bPAD(0)
	{
	}

	constexpr SSamplerState(int nFilter, bool bClamp)
		: m_nMinFilter(nFilter)
		, m_nMagFilter(nFilter)
		, m_nMipFilter(nFilter)
		, m_nAddressU(bClamp ? eSamplerAddressMode_Clamp : eSamplerAddressMode_Wrap)
		, m_nAddressV(bClamp ? eSamplerAddressMode_Clamp : eSamplerAddressMode_Wrap)
		, m_nAddressW(bClamp ? eSamplerAddressMode_Clamp : eSamplerAddressMode_Wrap)
		, m_nAnisotropy(ExtractAniso(nFilter))
		, m_nPadding(0)
		, m_dwBorderColor(0)
		, m_fMipLodBias(0.0f)
		, m_bActive(false)
		, m_bComparison(false)
		, m_bSRGBLookup(false)
		, m_bPAD(0)
	{
	}

	constexpr SSamplerState(int nFilter, ESamplerAddressMode nAddressU, ESamplerAddressMode nAddressV, ESamplerAddressMode nAddressW, unsigned int borderColor, bool bComparison = false)
		: m_nMinFilter(nFilter)
		, m_nMagFilter(nFilter)
		, m_nMipFilter(nFilter)
		, m_nAddressU(nAddressU)
		, m_nAddressV(nAddressV)
		, m_nAddressW(nAddressW)
		, m_nAnisotropy(ExtractAniso(nFilter))
		, m_nPadding(0)
		, m_dwBorderColor(borderColor)
		, m_fMipLodBias(0.0f)
		, m_bActive(false)
		, m_bComparison(bComparison)
		, m_bSRGBLookup(false)
		, m_bPAD(0)
	{
	}

	constexpr SSamplerState(const SSamplerState& src)
		: m_nMinFilter(src.m_nMinFilter)
		, m_nMagFilter(src.m_nMagFilter)
		, m_nMipFilter(src.m_nMipFilter)
		, m_nAddressU(src.m_nAddressU)
		, m_nAddressV(src.m_nAddressV)
		, m_nAddressW(src.m_nAddressW)
		, m_nAnisotropy(src.m_nAnisotropy)
		, m_nPadding(src.m_nPadding)
		, m_dwBorderColor(src.m_dwBorderColor)
		, m_fMipLodBias(0.0f)
		, m_bActive(src.m_bActive)
		, m_bComparison(src.m_bComparison)
		, m_bSRGBLookup(src.m_bSRGBLookup)
		, m_bPAD(src.m_bPAD)
	{
	}

	SSamplerState& operator=(const SSamplerState& src)
	{
		this->~SSamplerState();
		new(this)SSamplerState(src);
		return *this;
	}
	inline friend bool operator==(const SSamplerState& m1, const SSamplerState& m2)
	{
		return *(uint64*)&m1 == *(uint64*)&m2 && m1.m_dwBorderColor == m2.m_dwBorderColor && m1.m_fMipLodBias == m2.m_fMipLodBias &&
			   m1.m_bActive == m2.m_bActive && m1.m_bComparison == m2.m_bComparison && m1.m_bSRGBLookup == m2.m_bSRGBLookup;
	}
	void Release()
	{
		delete this;
	}

	bool SetFilterMode(int nFilter);
	void SetClampMode(ESamplerAddressMode nAddressU, ESamplerAddressMode nAddressV, ESamplerAddressMode nAddressW);
	void SetBorderColor(DWORD dwColor);
	void SetMipLodBias(float fMipLodBias);
	void SetComparisonFilter(bool bEnable);

	// TODO: deprecate global state based sampler state configuration
	static SSamplerState s_sDefState;
	static bool SetDefaultFilterMode(int nFilter);
	static void SetDefaultClampingMode(ESamplerAddressMode nAddressU, ESamplerAddressMode nAddressV, ESamplerAddressMode nAddressW);

private:
	static constexpr signed char ExtractAniso(int nFilter)
	{
		return nFilter == FILTER_ANISO2X ? 2 :
		       nFilter == FILTER_ANISO4X ? 4 :
		       nFilter == FILTER_ANISO8X ? 8 :
		       nFilter == FILTER_ANISO16X ? 16 :
		       0;
	}
};

// TODO: Move to DeviceObjects.h
struct EDefaultSamplerStates : SamplerStateHandle
{
	enum PreDefs : ValueType
	{
		PointClamp            = 0,
		PointWrap             = 1,
		PointBorder_Black     = 2,
		PointBorder_White     = 3,
		PointCompare          = 4,
		LinearClamp           = 5,
		LinearWrap            = 6,
		LinearBorder_Black    = 7,
		LinearCompare         = 8,
		BilinearClamp         = 9,
		BilinearWrap          = 10,
		BilinearBorder_Black  = 11,
		BilinearCompare       = 12,
		TrilinearClamp        = 13,
		TrilinearWrap         = 14,
		TrilinearBorder_Black = 15,
		TrilinearBorder_White = 16,

		PreAllocated          = 17 // from this value and up custom sampler states are assigned
	};
};
static_assert(sizeof(EDefaultSamplerStates::ValueType) == sizeof(EDefaultSamplerStates), "EDefaultSamplerStates is suppose to be as small as the base type");


////////////////////////////////////////////////////////////////////////////
// InputLayout API

struct SShaderBlob
{
	void* m_pShaderData;
	int   m_nDataSize;
};

struct SInputLayout
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> m_Declaration; // Configuration
	uint16                                m_firstSlot;
	std::vector<uint16>                   m_Strides; // Stride of each input slot, starting from m_firstSlot
	std::array<int8, 4>                   m_Offsets; // The offsets of "POSITION", "COLOR", "TEXCOORD" and "NORMAL"

	enum
	{
		eOffset_Position,
		eOffset_Color,
		eOffset_TexCoord,
		eOffset_Normal,
	};

	SInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC>&& decs)
	    : m_Declaration(std::move(decs))
	{
		// Calculate first slot index
		m_firstSlot = std::numeric_limits<uint16>::max();
		for (const auto& dec : m_Declaration)
			m_firstSlot = std::min(m_firstSlot, static_cast<uint16>(dec.InputSlot));

		// Calculate strides
		for (const auto& dec : m_Declaration)
		{
			const uint16 slot = dec.InputSlot - m_firstSlot;
			if (m_Strides.size() <= slot)
				m_Strides.resize(slot + 1, 0);

			m_Strides[slot] = std::max(m_Strides[slot], uint16(dec.AlignedByteOffset + DeviceFormats::GetStride(dec.Format)));
		}

		// Calculate offsets
		m_Offsets[eOffset_Position] = m_Offsets[eOffset_Color] = m_Offsets[eOffset_TexCoord] = m_Offsets[eOffset_Normal] = -1;
		for (int n = 0; n < m_Declaration.size(); ++n)
		{
			if (!m_Declaration[n].SemanticName)
				continue;

			if ((m_Offsets[eOffset_Position] == -1) && (!stricmp(m_Declaration[n].SemanticName, "POSITION")))
				m_Offsets[eOffset_Position] = m_Declaration[n].AlignedByteOffset;
			if ((m_Offsets[eOffset_Color] == -1) && (!stricmp(m_Declaration[n].SemanticName, "COLOR")))
				m_Offsets[eOffset_Color] = m_Declaration[n].AlignedByteOffset;
			if ((m_Offsets[eOffset_TexCoord] == -1) && (!stricmp(m_Declaration[n].SemanticName, "TEXCOORD")))
				m_Offsets[eOffset_TexCoord] = m_Declaration[n].AlignedByteOffset;
			if ((m_Offsets[eOffset_Normal] == -1) && (!stricmp(m_Declaration[n].SemanticName, "NORMAL") || !stricmp(m_Declaration[n].SemanticName, "TANGENT")))
				m_Offsets[eOffset_Normal] = m_Declaration[n].AlignedByteOffset;
		}
	}

	SInputLayout(const SInputLayout& src) = default;
	SInputLayout(SInputLayout&& src)      = default;
	SInputLayout& operator=(const SInputLayout& src) = default;
	SInputLayout& operator=(SInputLayout&& src) = default;

	bool          operator==(const std::vector<D3D11_INPUT_ELEMENT_DESC>& descs) const
	{
		size_t count = m_Declaration.size();
		if (count != descs.size())
		{
			return false;
		}

		for (size_t i = 0; i < count; ++i)
		{
			const D3D11_INPUT_ELEMENT_DESC& desc0 = m_Declaration[i];
			const D3D11_INPUT_ELEMENT_DESC& desc1 = descs[i];

			if (0 != stricmp(desc0.SemanticName, desc1.SemanticName) ||
			    desc0.SemanticIndex != desc1.SemanticIndex ||
			    desc0.Format != desc1.Format ||
			    desc0.InputSlot != desc1.InputSlot ||
			    desc0.AlignedByteOffset != desc1.AlignedByteOffset ||
			    desc0.InputSlotClass != desc1.InputSlotClass ||
			    desc0.InstanceDataStepRate != desc1.InstanceDataStepRate)
			{
				return false;
			}
		}
		return true;
	}
};

const D3D11_COMPARISON_FUNC REVERSED_ZBUFFER_FUNC = D3D11_COMPARISON_GREATER;
const D3D11_COMPARISON_FUNC ORDINARY_ZBUFFER_FUNC = D3D11_COMPARISON_LESS;

const float REVERSED_ZBUFFER_CLEAR_VALUE = 0.f;
const float ORDINARY_ZBUFFER_CLEAR_VALUE = 1.f;

#define USE_REVERSED_Z_BUFFER
#ifdef USE_REVERSED_Z_BUFFER
const D3D11_COMPARISON_FUNC ZBUFFER_FUNC = REVERSED_ZBUFFER_FUNC;
const float ZBUFFER_CLEAR_VALUE = REVERSED_ZBUFFER_CLEAR_VALUE;
#else
const D3D11_COMPARISON_FUNC ZBUFFER_FUNC = ORDINARY_ZBUFFER_FUNC;
const float ZBUFFER_CLEAR_VALUE = ORDINARY_ZBUFFER_CLEAR_VALUE;
#endif

