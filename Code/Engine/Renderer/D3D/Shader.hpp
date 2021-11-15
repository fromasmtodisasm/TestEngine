#pragma once
#include <BlackBox/Core/MathHelper.hpp>
#include <BlackBox/Renderer/IRender.hpp>
#include <BlackBox/Renderer/VertexFormats.hpp>
#include <BlackBox/System/ISystem.hpp>
#include <BlackBox/Utils/smartptr.hpp>

#include "../Shaders/Effect.hpp"
#include <d3dcompiler.h>

enum class ShaderBinaryFormat
{
	SPIRV  = 1,
	VENDOR = 1 << 2
};

struct ICVar;

using VertexShader	 = ID3D11VertexShader;
using GeometryShader = ID3D11GeometryShader;
using PixelShader	 = ID3D11PixelShader;

using PVertexShader	  = ID3D11VertexShader*;
using PGeometryShader = ID3D11GeometryShader*;
using PPixelShader	  = ID3D11PixelShader*;

class CHWShader;

enum ED3DShError
{
	ED3DShError_NotCompiled,
	ED3DShError_CompilingError,
	ED3DShError_Fake,
	ED3DShError_Ok,
	ED3DShError_Compiling,
};

class CHWShader : public NoCopy, public _reference_target_t
{
  public:
	EHWShaderClass            m_eSHClass;
	//EHWSProfile m_eHWProfile;

	static class CHWShader* s_pCurHWVS;
	static char *s_GS_MultiRes_NV;

	string                    m_Name;
	string                    m_NameSourceFX;
	string                    m_EntryFunc;
	static CHWShader*  mfForName(const char* name, const char* nameSource, const char* szEntryFunc, EHWShaderClass eClass, CShader* pFX, uint64 nMaskGen = 0, uint64 nMaskGenFX = 0);
};

// Shader pass definition for HW shaders
struct SShaderPass
{
	std::string Name;
	uint32      m_RenderState;     // Render state flags
	signed char m_eCull;
	uint8       m_AlphaRef;

	uint16      m_PassFlags;         // Different usefull Pass flags (SHPF_)

	CHWShader*  m_VShader;        // Pointer to the vertex shader for the current pass
	CHWShader*  m_PShader;        // Pointer to fragment shader
	CHWShader*  m_GShader;        // Pointer to the geometry shader for the current pass
	CHWShader*  m_DShader;        // Pointer to the domain shader for the current pass
	CHWShader*  m_HShader;        // Pointer to the hull shader for the current pass
	CHWShader*  m_CShader;        // Pointer to the compute shader for the current pass
	SShaderPass();

	int Size()
	{
		int nSize = sizeof(SShaderPass);
		return nSize;
	}

	void GetMemoryUsage(ICrySizer* pSizer) const
	{
		#if 0
		pSizer->AddObject(m_VShader);
		pSizer->AddObject(m_PShader);
		pSizer->AddObject(m_GShader);
		pSizer->AddObject(m_HShader);
		pSizer->AddObject(m_DShader);
		pSizer->AddObject(m_CShader);
		#endif
	}
	void mfFree()
	{
		SAFE_RELEASE(m_VShader);
		SAFE_RELEASE(m_PShader);
		SAFE_RELEASE(m_GShader);
		SAFE_RELEASE(m_HShader);
		SAFE_RELEASE(m_DShader);
		SAFE_RELEASE(m_CShader);
	}

	void AddRefsToShaders()
	{
		if (m_VShader)
			m_VShader->AddRef();
		if (m_PShader)
			m_PShader->AddRef();
		if (m_GShader)
			m_GShader->AddRef();
		if (m_DShader)
			m_DShader->AddRef();
		if (m_HShader)
			m_HShader->AddRef();
		if (m_CShader)
			m_CShader->AddRef();
	}

private:
	SShaderPass& operator=(const SShaderPass& sl);
};


class SD3DShader
{
	friend struct SD3DShaderHandle;

	d3dShaderHandleType* m_pHandle = nullptr;
	EHWShaderClass		 m_eSHClass;
	std::size_t			 m_nSize;

	std::size_t m_refCount = 0;

	SD3DShader(d3dShaderHandleType* handle, EHWShaderClass eSHClass, std::size_t size)
		: m_pHandle(handle)
		, m_eSHClass(eSHClass)
		, m_nSize(size)
	{
	}

  public:
	~SD3DShader() noexcept;
	SD3DShader(const SD3DShader&) = delete;
	SD3DShader& operator=(const SD3DShader&) = delete;

	void* GetHandle() const { return m_pHandle; }
	void  GetMemoryUsage(ICrySizer* pSizer) const { pSizer->AddObject(this, sizeof(*this)); }

	void		AddRef() { ++m_refCount; }
	std::size_t Release()
	{
		const auto referencesLeft = --m_refCount;
		if (referencesLeft == 0)
			delete this;

		return referencesLeft;
	}

	bool m_bDisabled = false;
};

struct SD3DShaderHandle
{
	_smart_ptr<SD3DShader> m_pShader;

	byte* m_pData	= nullptr;
	int	  m_nData	= 0;
	byte  m_bStatus = 0;

	SD3DShaderHandle() = default;
	SD3DShaderHandle(_smart_ptr<SD3DShader>&& handle) noexcept
		: m_pShader(std::move(handle))
	{
	}
	SD3DShaderHandle(d3dShaderHandleType* handle, EHWShaderClass eSHClass, std::size_t size) noexcept
		: m_pShader(new SD3DShader(handle, eSHClass, size))
	{
	}
	SD3DShaderHandle(const SD3DShaderHandle&)	  = default;
	SD3DShaderHandle(SD3DShaderHandle&&) noexcept = default;
	SD3DShaderHandle& operator=(const SD3DShaderHandle&) = default;
	SD3DShaderHandle& operator=(SD3DShaderHandle&&) noexcept = default;

	void SetFake()
	{
		m_bStatus = 2;
	}
	void SetNonCompilable()
	{
		m_bStatus = 1;
	}

	void GetMemoryUsage(ICrySizer* pSizer) const
	{
		#if 0
		pSizer->AddObject(m_pShader);
		#endif
	}
};

class CHWShader_D3D : public CHWShader
{
  public:
	friend class CHWShader;
	friend class CShaderManager;
	struct SHWSInstance
	{
		friend struct SShaderAsyncInfo;

		SShaderBlob m_Shader;

		SD3DShaderHandle m_Handle;
		EHWShaderClass	 m_eClass;

		EStreamMasks	  m_VStreamMask_Stream;
		EStreamMasks	  m_VStreamMask_Decl;
		InputLayoutHandle m_nVertexFormat;
	};

  public:
	SHWSInstance*		 m_pCurInst{};

	CHWShader_D3D()
	{
		//auto r = m_D3DShader->Release();
	}

	void Bind()
	{
	}
	ED3DShError mfFallBack(SHWSInstance*& pInst, int nStatus)
	{
		return ED3DShError_CompilingError;
	}
	#if 0
	//ILINE most common outcome (avoid LHS on link register 360)
	ILINE ED3DShError mfIsValid(SHWSInstance*& pInst, bool bFinalise)
	{
		if (pInst->m_Handle.m_pShader)
			return ED3DShError_Ok;
		if (pInst->m_bAsyncActivating)
			return ED3DShError_NotCompiled;

		return mfIsValid_Int(pInst, bFinalise);
	}
	#endif
	bool CHWShader_D3D::mfActivate(CShader* pSH, uint32 nFlags);

	bool Upload(ID3DBlob* pBlob, CShader* pSH);
	void mfPostVertexFormat(SHWSInstance* pInst, CHWShader_D3D* pHWSH, bool bCol, byte bNormal, bool bTC0, bool bTC1[2], bool bPSize, bool bTangent[2], bool bBitangent[2], bool bHWSkin, bool bSH[2], bool bVelocity, bool bMorph)
	{
		if (bBitangent[0])
			pInst->m_VStreamMask_Decl |= EStreamMasks(1 << VSF_TANGENTS);
		else if (bTangent[0])
			pInst->m_VStreamMask_Decl |= EStreamMasks(1 << VSF_QTANGENTS);
		if (bBitangent[1])
			pInst->m_VStreamMask_Stream |= EStreamMasks(1 << VSF_TANGENTS);
		else if (bTangent[1])
			pInst->m_VStreamMask_Stream |= EStreamMasks(1 << VSF_QTANGENTS);

		if (pInst->m_VStreamMask_Decl & EStreamMasks(1 << VSF_TANGENTS))
			bNormal = false;

		if (bHWSkin)
		{
			pInst->m_VStreamMask_Decl |= VSM_HWSKIN;
			pInst->m_VStreamMask_Stream |= VSM_HWSKIN;
		}

		if (bVelocity)
		{
			pInst->m_VStreamMask_Decl |= VSM_VERTEX_VELOCITY;
			pInst->m_VStreamMask_Stream |= VSM_VERTEX_VELOCITY;
		}
		if (bMorph)
		{
			pInst->m_VStreamMask_Decl |= VSM_MORPHBUDDY;
			pInst->m_VStreamMask_Stream |= VSM_MORPHBUDDY;
		}

		InputLayoutHandle eVF  = VertFormatForComponents(bCol, bTC0, bPSize, bNormal != 0);
		pInst->m_nVertexFormat = eVF;
	}
	InputLayoutHandle mfVertexFormat(SHWSInstance* pInst, CHWShader_D3D* pSH, D3DBlob* pShader, void* pConstantTable)
	{
		/*if (!stricmp(pSH->m_EntryFunc.c_str(), "ParticleVS"))
	   {
	   int nnn = 0;
	   }*/

		assert(pSH->m_eSHClass == IShader::Type::E_VERTEX);

		byte bNormal	   = false;
		bool bTangent[2]   = {false, false};
		bool bBitangent[2] = {false, false};
		bool bHWSkin	   = false;
		bool bVelocity	   = false;
		bool bMorph		   = false;
		bool bPSize		   = false;
		bool bSH[2]		   = {false, false};
		bool bTC0		   = false;
		bool bTC1[2]	   = {false, false};
		bool bCol		   = false;
		bool bSecCol	   = false;
		bool bPos		   = false;

		size_t	nSize		   = pShader->GetBufferSize();
		void*	pData		   = pShader->GetBufferPointer();
		void*	pShaderReflBuf = pConstantTable;
		HRESULT hr			   = pConstantTable ? S_OK : D3DReflection(pData, nSize, IID_D3DShaderReflection, &pShaderReflBuf);
		assert(SUCCEEDED(hr));
		D3DShaderReflection* pShaderReflection = (D3DShaderReflection*)pShaderReflBuf;
		if (!SUCCEEDED(hr))
			return InputLayoutHandle::Unspecified;
		D3D_SHADER_DESC Desc;
		pShaderReflection->GetDesc(&Desc);
		if (!Desc.InputParameters)
			return InputLayoutHandle::Unspecified;
		D3D_SIGNATURE_PARAMETER_DESC IDesc;
		for (uint32 i = 0; i < Desc.InputParameters; i++)
		{
			pShaderReflection->GetInputParameterDesc(i, &IDesc);
			//if (!IDesc.ReadWriteMask)
			//  continue;
			if (!IDesc.SemanticName)
				continue;
			int nIndex;
			if (!strnicmp(IDesc.SemanticName, "POSITION", 8) || !strnicmp(IDesc.SemanticName, "SV_POSITION", 11))
			{
				nIndex = IDesc.SemanticIndex;
				if (nIndex == 0)
					bPos = true;
				else if (nIndex == 3)
					bVelocity = true;
				else if (nIndex == 4)
					bHWSkin = true;
				else if (nIndex == 8)
					bMorph = true;
				else
					assert(false);
			}
			else if (!strnicmp(IDesc.SemanticName, "NORMAL", 6))
			{
				bNormal = true;
			}
			else if (!strnicmp(IDesc.SemanticName, "TEXCOORD", 8))
			{
				nIndex = IDesc.SemanticIndex;
				if (nIndex == 0)
					bTC0 = true;
				else
				{
					if (nIndex == 1)
					{
						bTC1[0] = true;
						if (IDesc.ReadWriteMask)
							bTC1[1] = true;
					}
					else if (nIndex == 8)
						bMorph = true;
				}
			}
			else if (!strnicmp(IDesc.SemanticName, "COLOR", 5))
			{
				nIndex = IDesc.SemanticIndex;
				if (nIndex == 0)
					bCol = true;
				else if (nIndex == 1)
					bSecCol = true;
				else
				{
					if (nIndex == 2 || nIndex == 3)
					{
						bSH[0] = true;
						if (IDesc.ReadWriteMask)
							bSH[1] = true;
					}
					else
						assert(false);
				}
			}
			else if (!stricmp(IDesc.SemanticName, "TANGENT"))
			{
				bTangent[0] = true;
				if (IDesc.ReadWriteMask)
					bTangent[1] = true;
			}
			else if (!stricmp(IDesc.SemanticName, "BITANGENT"))
			{
				bBitangent[0] = true;
				if (IDesc.ReadWriteMask)
					bBitangent[1] = true;
			}
			else if (!strnicmp(IDesc.SemanticName, "PSIZE", 5) || !strnicmp(IDesc.SemanticName, "AXIS", 4))
			{
				bPSize = true;
			}
			else if (!strnicmp(IDesc.SemanticName, "BLENDWEIGHT", 11) || !strnicmp(IDesc.SemanticName, "BLENDINDICES", 12))
			{
				nIndex = IDesc.SemanticIndex;
				if (nIndex == 0)
					bHWSkin = true;
				else if (nIndex == 1)
					bMorph = true;
				else
					assert(0);
			}
			else if (!strnicmp(IDesc.SemanticName, "SV_", 3))
			{
				// SV_ are valid semantics
			}
#if CRY_PLATFORM_ORBIS
			else if (!strnicmp(IDesc.SemanticName, "S_VERTEX_ID", 11) || !strnicmp(IDesc.SemanticName, "S_INSTANCE_ID", 13))
			{
				// S_VERTEX_ID and S_INSTANCE_ID are valid names
			}
#endif
			else
			{
				CRY_ASSERT(0, "Invalid SemanticName %s", IDesc.SemanticName);
			}

#if CRY_RENDERER_VULKAN
			pInst->m_VSInputStreams.emplace_back(IDesc.SemanticName, IDesc.SemanticIndex, IDesc.AttributeLocation);
#endif
		}

		mfPostVertexFormat(pInst, pSH, bCol, bNormal, bTC0, bTC1, bPSize, bTangent, bBitangent, bHWSkin, bSH, bVelocity, bMorph);

		if (pConstantTable != pShaderReflection)
		{
			SAFE_RELEASE(pShaderReflection);
		}

		return (InputLayoutHandle)pInst->m_nVertexFormat;
	}

	bool mfCreateShaderEnv(SHWSInstance* pInst, D3DBlob* pShader, void*& pConstantTable, D3DBlob*& pErrorMsgs, CHWShader_D3D* pSH, CShader* pFXShader)
	{
		// Create asm (.fxca) cache file
		assert(pInst);
		if (!pInst)
			return false;
		if (pShader)
		{
			bool bVF = pSH->m_eSHClass == eHWSC_Vertex;
			if (bVF)
				mfVertexFormat(pInst, pSH, pShader, pConstantTable);
#if 0
			if (pConstantTable)
				mfCreateBinds(pInst->m_pBindVars, pConstantTable, (std::size_t)pShader->GetBufferSize());
#endif
		}
		int nConsts = 0;

		SAFE_RELEASE(pErrorMsgs);
		if (D3DShaderReflection* pShaderReflection = (D3DShaderReflection*)pConstantTable)
		{
			pShaderReflection->Release();
			pConstantTable = nullptr;
		}

		return true;
	}
};

class CShader : public IShader
{
	friend class CShaderManager;

  public:
	// Inherited via IShader
	~CShader();

	CShader& operator=(const CShader& src);

	virtual int			  GetID() override;
	virtual void		  AddRef() override;
	virtual void		  Release(bool bForce = false);
	virtual IShader::Type GetType() override;
	virtual const char*	  GetName() override;
	virtual void		  Bind() override;
	virtual int			  GetFlags() override;
	virtual int			  GetFlags2() override;

	virtual DynVertexFormat* GetDynVertexFormat()
	{
#if 0
		return &format;
#else
		return nullptr;
#endif
	}

	bool WaitUntilLoaded();

	void			SaveBinaryShader(std::string_view name, int flags, uint64 nMaskGen);
	static CShader* LoadBinaryShader(std::string_view name, int flags, uint64 nMaskGen);

	void CreateInputLayout();
	void ReflectShader();

	static void							   LoadShader(SShaderPass* pass, EHWShaderClass st, std::vector<std::string>& code, CShader* pSH);
	static const char*					   mfProfileString(EHWShaderClass type);
	static std::pair<ID3DBlob*, ID3DBlob*> Load(const std::string_view text, EHWShaderClass type, const char* pEntry, bool bFromMemory);
	static CHWShader_D3D*				   LoadFromFile(const std::string_view text, IShader::Type type, const char* pEntry);
	static std::pair<ID3DBlob*, ID3DBlob*> LoadFromMemory(const std::vector<std::string>& text, EHWShaderClass type, const char* pEntry);

	string m_NameShader;
	string m_NameFile;
	int	   m_NumRefs = 0;
	int	   m_Flags	 = 0;
	int	   m_Flags2	 = 0;

	ID3D11InputLayout*						m_pInputLayout;
	D3D11_SHADER_DESC						m_Desc;
	ID3D11ShaderReflection*					m_pReflection;
	DynVertexFormat							format;
	std::array<CHWShader_D3D*, Type::E_NUM> m_Shaders{0};
};
