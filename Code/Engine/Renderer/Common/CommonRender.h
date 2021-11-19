#pragma once

#include <BlackBox/Core/CryEnumMacro.h>
#include "CryNameR.h"

//////////////////////////////////////////////////////////////////////
class CRenderer;
extern CRenderer* gRenDev;

class CShader;
class CBaseResource;

struct GlobalResources
{
	static ID3DShaderResourceView* FontAtlasRV;

	static ID3DShaderResourceView* WiteTextureRV;
	static ID3DShaderResourceView* GreyTextureRV;

	static ID3D11SamplerState* LinearSampler;

	static ID3D10EffectTechnique* BoxTechnique;
	static ID3D10EffectTechnique* MeshTechnique;

	static ID3DInputLayout* VERTEX_FORMAT_P3F_C4B_T2F_Layout;

	static _smart_ptr<CShader> SpriteShader;
	static _smart_ptr<CShader> TexturedQuadShader;

	//static _smart_ptr<CShader> GrayScale




	static ID3D11BlendState* FontBlendState;

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

}


////////////////////////////////////////////////////////////////////////////
// InputLayout API

struct SShaderBlob
{
	void* m_pShaderData;
	int   m_nDataSize;
};

struct SInputLayout
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> m_Declaration;			 // Configuration
	uint16                                m_firstSlot;
	std::vector<uint16>                   m_Strides;				 // Stride of each input slot, starting from m_firstSlot
	std::array<int8, 4>                   m_Offsets;				 // The offsets of "POSITION", "COLOR", "TEXCOORD" and "NORMAL"

	enum
	{
		eOffset_Position,
		eOffset_Color,
		eOffset_TexCoord,
		eOffset_Normal,
	};

	SInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC> &&decs) : m_Declaration(std::move(decs))
	{
		// Calculate first slot index
		m_firstSlot = std::numeric_limits<uint16>::max();
		for (const auto &dec : m_Declaration)
			m_firstSlot = std::min(m_firstSlot, static_cast<uint16>(dec.InputSlot));

		// Calculate strides
		for (const auto &dec : m_Declaration)
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
	SInputLayout(SInputLayout&& src) = default;
	SInputLayout& operator=(const SInputLayout& src) = default;
	SInputLayout& operator=(SInputLayout&& src) = default;

	bool operator==(const std::vector<D3D11_INPUT_ELEMENT_DESC>& descs) const
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

#if 0 
#if 0
typedef CCryNameR CCryNameTSCRC;
#else
typedef string CCryNameTSCRC;
#endif
#endif

namespace stl
{
	template<class T>
	using STLGlobalAllocator = std::allocator<T>;
}

////////////////////////////////////////////////////////////////////////////
// Resource and Resource-Directory API

typedef std::map<CCryNameTSCRC, CBaseResource*> ResourcesMap;
typedef ResourcesMap::iterator					ResourcesMapItor;

typedef std::vector<CBaseResource*, stl::STLGlobalAllocator<CBaseResource*>> ResourcesList;
typedef std::vector<int, stl::STLGlobalAllocator<int>>						 ResourceIds;

struct SResourceContainer
{
	ResourcesList m_RList;		  // List of objects for acces by Id's
	ResourcesMap  m_RMap;		  // Map of objects for fast searching
	ResourceIds	  m_AvailableIDs; // Available object Id's for efficient ID's assigning after deleting

	SResourceContainer()
	{
		m_RList.reserve(512);
	}

	~SResourceContainer();

	void GetMemoryUsage(ICrySizer* pSizer) const
	{
		pSizer->AddObject(this, sizeof(*this));
		pSizer->AddObject(m_RList);
		pSizer->AddObject(m_RMap);
		pSizer->AddObject(m_AvailableIDs);
	}
};


typedef std::map<CCryNameTSCRC, SResourceContainer*> ResourceClassMap;
typedef ResourceClassMap::iterator					 ResourceClassMapItor;

class CBaseResource : NoCopy
	//, public CResourceBindingInvalidator
{
  private:
	// Per resource variables
	volatile int32 m_nRefCount;
	int			   m_nID;
	CCryNameTSCRC  m_ClassName;
	CCryNameTSCRC  m_NameCRC;

	static ResourceClassMap m_sResources;

	bool m_bDeleted = false;

  public:
#if 0
	static CryRWLock s_cResLock;
#endif

  private:
	void UnregisterAndDelete();

  public:
	// CCryUnknown interface
	inline void	  SetRefCounter(int nRefCounter) { m_nRefCount = nRefCounter; }
	virtual int	  GetRefCounter() const { return m_nRefCount; }
	virtual int32 AddRef()
	{
		int32 nRef = CryInterlockedIncrement(&m_nRefCount);
		return nRef;
	}
	virtual int32 Release()
	{
		// TODO: simplify, it's making ref-counting on CTexture much more expensive than it needs to be
		IF(m_nRefCount > 0, 1)
		{
			int32 nRef = CryInterlockedDecrement(&m_nRefCount);
			if (nRef < 0)
			{
				CryFatalError("CBaseResource::Release() called more than once!");
			}

			if (nRef == 0)
			{
				UnregisterAndDelete();
				return 0;
			}
			return nRef;
		}
		return 0;
	}

	// Increment ref count, if not already scheduled for destruction.
	#if 0
	int32 TryAddRef()
	{
		volatile int nOldRef, nNewRef;
		do
		{
			nOldRef = m_nRefCount;
			if (nOldRef == 0)
				return 0;
			nNewRef = nOldRef + 1;
		} while (CryInterlockedCompareExchange(alias_cast<volatile LONG*>(&m_nRefCount), nNewRef, nOldRef) != nOldRef);
		return nNewRef;
	}
	#endif

	// Constructors.
	CBaseResource()
		: m_nRefCount(1)
		, m_nID(0)
	{
	}

	// Destructor.
	virtual ~CBaseResource(){};

	CCryNameTSCRC GetNameCRC() const { return m_NameCRC; }
	//inline const char *GetName() const { return m_Name.c_str(); }
	//inline const char *GetClassName() const { return m_ClassName.c_str(); }
	inline int	GetID() const { return m_nID; }
	inline void SetID(int nID) { m_nID = nID; }

	virtual bool IsValid() const;

	static ILINE int RListIndexFromId(int id) { return id - 1; }
	static ILINE int IdFromRListIndex(int idx) { return idx + 1; }

	static ResourceClassMap&   GetMaps() { return m_sResources; }
	static CBaseResource*	   GetResource(const CCryNameTSCRC& className, int nID, bool bAddRef);
	static CBaseResource*	   GetResource(const CCryNameTSCRC& className, const CCryNameTSCRC& Name, bool bAddRef);
	static SResourceContainer* GetResourcesForClass(const CCryNameTSCRC& className);
	static void				   ShutDown();

	bool Register(const CCryNameTSCRC& resName, const CCryNameTSCRC& Name);
	bool UnRegister();

	virtual void GetMemoryUsage(ICrySizer* pSizer) const = 0;
};


