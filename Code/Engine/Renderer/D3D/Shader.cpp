#include "Shader.hpp"
#include "Shader.hpp"
#include <BlackBox/System/FrameProfiler.hpp>
#include "Renderer.h"
#include <BlackBox/Core/Utils.hpp>
#include <filesystem>
#include <fstream>
#include <string_view>

#include <d3dcompiler.h>

namespace fs = std::filesystem;
#define PROFILE_FRAME(s) FUNCTION_PROFILER(PROFILE_RENDERER)

CShader::~CShader()
{
	#if 0
	for (auto s : m_Shaders)
	{
		if (s)
		{
			auto sh = s->m_D3DShader;
			SAFE_RELEASE(sh);
		}
	}
	#endif
}

CShader& CShader::operator=(const CShader& src)
{
	uint32 i;

	//mfFree();

	int	  Offs = (int)(INT_PTR) & (((CShader*)0)->m_NumRefs);
	byte* d	   = (byte*)this;
	byte* s	   = (byte*)&src;
	memcpy(&d[Offs], &s[Offs], sizeof(CShader) - Offs);

	m_NameShader	 = src.m_NameShader;
	m_NameFile		 = src.m_NameFile;
	//m_NameShaderICRC = src.m_NameShaderICRC;

	#if 1
	if (src.m_HWTechniques.Num())
	{
		m_HWTechniques.Create(src.m_HWTechniques.Num());
		for (i = 0; i < src.m_HWTechniques.Num(); i++)
		{
			m_HWTechniques[i]			= new SShaderTechnique(this);
			*m_HWTechniques[i]			= *src.m_HWTechniques[i];
			m_HWTechniques[i]->m_shader = this; // copy operator will override m_shader
		}
	}
	#endif

	return *this;
}

int CShader::GetID()
{
	return 0;
}

void CShader::AddRef()
{
	m_NumRefs++;
}

void CShader::Release(bool bForce)
{
	m_NumRefs--;
	auto refs = m_NumRefs;
	if (m_NumRefs <= 0)
		delete this;
}

IShader::Type CShader::GetType()
{
	return IShader::Type();
}

const char* CShader::GetName()
{
	return m_NameShader.c_str();
}

#if 0
eVertexFormat CShader::GetVertexFormat(void)
{
	return eVertexFormat();
}
#endif

void CShader::Bind()
{
	if ((m_Flags2 & EF2_FAILED) != 0)
		return;
	auto d = GetDeviceContext();

	if (!WaitUntilLoaded())
		return;
	if (auto s = m_Shaders[E_VERTEX]->m_pCurInst->m_Handle.m_pShader->GetHandle(); s) { d->VSSetShader((PVertexShader)s, nullptr, 0); }
	//if (auto s = m_Shaders[E_GEOMETRY]->m_D3DShader; s) { d->VSSetShader((PVertexShader)s); }
	if (auto s = m_Shaders[E_FRAGMENT]->m_pCurInst->m_Handle.m_pShader->GetHandle(); s) { d->PSSetShader((PPixelShader)s, nullptr, 0); }

	d->IASetInputLayout(m_pInputLayout);
}

int CShader::GetFlags()
{
	return m_Flags;
}

int CShader::GetFlags2()
{
	return 0;
}

bool CShader::WaitUntilLoaded()
{
	if ((m_Flags2 & EF2_LOADED) == 0)
	{
		while ((m_Flags2 & (EF2_LOADED | EF2_FAILED)) == 0)
		{
			Sleep(1);
		}
	}
	return (m_Flags2 & EF2_FAILED) == 0;
}

void CShader::SaveBinaryShader(std::string_view name, int flags, uint64 nMaskGen)
{
}

CShader* CShader::LoadBinaryShader(std::string_view name, int flags, uint64 nMaskGen)
{
	return nullptr;
}

template<class T, size_t size>
struct PaddedStruct : public T
{
	std::array<uint8_t, size> Array;
};

void CShader::CreateInputLayout()
{
	HRESULT hr{};

	auto								  pVSBuf = (ID3DBlob*)m_Shaders[IShader::Type::E_VERTEX]->m_pCurInst->m_Shader.m_pShaderData;
	std::vector<D3D11_INPUT_ELEMENT_DESC> t_InputElementDescVec; // actually does not matter what store
	unsigned int						  t_ByteOffset = 0;

	for (int i = 0; i < m_Desc.InputParameters; ++i)
	{
		PaddedStruct<D3D11_SIGNATURE_PARAMETER_DESC, 100> SP_DESC;
		ZeroStruct(SP_DESC);
		m_pReflection->GetInputParameterDesc(i, &SP_DESC);

		// FIXME: temporary hack to workaround stack couruption around SP_DESC, because D3D11_INPUT_ELEMENT_DESC
		// struct has new field in last version of d3d11
		//
		// During call D3DReflect we get pointer to newer interface of relfection (d3dcompiler_47.dll)
		// but in dxsdk 2010 june version d3dcompiler_43.dll, but if linked with 43 version, d3dreflect
		// not found coresponding interface for IID_ID3D11ShaderReflection!!! it is worked, but...

		D3D11_INPUT_ELEMENT_DESC t_InputElementDesc;
		t_InputElementDesc.SemanticName			= SP_DESC.SemanticName;
		t_InputElementDesc.SemanticIndex		= SP_DESC.SemanticIndex;
		t_InputElementDesc.InputSlot			= 0;
		t_InputElementDesc.AlignedByteOffset	= t_ByteOffset;
		t_InputElementDesc.InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
		t_InputElementDesc.InstanceDataStepRate = 0;

		if (SP_DESC.Mask == 1)
		{
			if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32_UINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32_SINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			t_ByteOffset += 4;
		}
		else if (SP_DESC.Mask <= 3)
		{
			if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			t_ByteOffset += 8;
		}
		else if (SP_DESC.Mask <= 7)
		{
			if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			t_ByteOffset += 12;
		}
		else if (SP_DESC.Mask <= 15)
		{
			bool isColor = false;
			uint offset	 = 16;

			if (!strcmp(SP_DESC.SemanticName, "COLOR"))
			{
				offset	= 4;
				isColor = true;
			}
			if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				t_InputElementDesc.Format = isColor ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			t_ByteOffset += offset;
		}

		t_InputElementDescVec.push_back(t_InputElementDesc);
	}
	hr = GetDevice()->CreateInputLayout(
		t_InputElementDescVec.data(),
		t_InputElementDescVec.size(),
		pVSBuf->GetBufferPointer(),
		pVSBuf->GetBufferSize(),
		&m_pInputLayout);
	if (FAILED(hr))
	{
		CryError("Error create input layout for font");
		//return false;
	}
	//SAFE_RELEASE(m_pReflection);
}

#if 0
void CShader::ReflectShader()
{
	HRESULT hr{};

	auto pVSBuf = (ID3DBlob*)m_Shaders[IShader::Type::E_VERTEX]->m_ByteCode;
	//ID3D11ShaderReflection* pIShaderReflection1 = NULL;
	hr = D3DReflect((void*)pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_pReflection);
	if (m_pReflection)
	{
		m_pReflection->GetDesc(&m_Desc);
	}
}
#endif
CHWShader* GetHWShader(EHWShaderClass type, SShaderPass& pass)
{
	switch (type)
	{
	case eHWSC_Vertex:
		return pass.m_VShader;
	case eHWSC_Pixel:
		return pass.m_PShader;
	case eHWSC_Geometry:
		return pass.m_GShader;
	case eHWSC_Domain:
		return pass.m_DShader;
	case eHWSC_Hull:
		return pass.m_HShader;
	case eHWSC_Compute:
		return pass.m_CShader;
	default:
		return nullptr;
	}
}

void CShader::LoadShaderPass(SShaderPass* pass, EHWShaderClass st, std::vector<std::string>& code, CShader* pSH)
{
	auto entry = GetHWShader(st, *pass)->m_EntryFunc;
	if (auto res = LoadFromMemory(code, st, entry.c_str()); res.first)
	{
		auto shader		   = new CHWShader_D3D;
		shader->m_eSHClass = st; 
		shader->m_EntryFunc = entry.c_str();
		shader->m_pCurInst->m_Handle =  SD3DShaderHandle(res.first, st, res.first->GetBufferSize());
		if (shader->Upload(shader->m_pCurInst, (ID3DBlob*)shader->m_pCurInst->m_Handle.m_pShader->GetHandle(), pSH))
		{
			pSH->m_Shaders[st] = shader;
		}
		else
		{
			res.second->Release();
			delete shader;
		}
	}
}

std::pair<ID3DBlob*,ID3DBlob*> CShader::LoadFromMemory(const std::vector<std::string>& text, EHWShaderClass type, const char* pEntry)
{
	std::string code;
	for (const auto& piece : text)
		code += piece;
	//return CShader::Load(code, type, pEntry, true);
	return std::make_pair(nullptr,nullptr);
}

#if 0
CHWShader_D3D* CShader::LoadFromFile(const std::string_view file, IShader::Type type, const char* pEntry)
{
	return CShader::Load(file, type, pEntry, false);
}
#endif

const char* GetGLSLANGTargetName(IShader::Type target)
{
	switch (target)
	{
	case IShader::E_VERTEX:
		return "vert";

	case IShader::E_GEOMETRY:
		return "geom";

	case IShader::E_FRAGMENT:
		return "frag";
	default:
		return "1111";
	}
}

void SaveHlslToDisk(const std::vector<std::string>& code, IShader::Type type)
{
	string									stage(GetGLSLANGTargetName(type));
	std::ofstream							output_file(string("bin/shadercache/shader_") + stage + ".hlsl");
	std::ostream_iterator<std::string_view> output_iterator(output_file, "\n");
	std::copy(code.begin(), code.end(), output_iterator);
	output_file.close();
}

CCryNameTSCRC CHWShader::mfGetClassName(EHWShaderClass eClass)
{
	static const auto s_sClassNameVertex = CCryNameTSCRC("CHWShader_VS");
	static const auto s_sClassNamePixel = CCryNameTSCRC("CHWShader_PS");
	static const auto s_sClassNameGeometry = CCryNameTSCRC("CHWShader_GS");
	static const auto s_sClassNameDomain = CCryNameTSCRC("CHWShader_DS");
	static const auto s_sClassNameHull = CCryNameTSCRC("CHWShader_HS");
	static const auto s_sClassNameCompute = CCryNameTSCRC("CHWShader_CS");

	switch (eClass)
	{
	case eHWSC_Vertex:
		return s_sClassNameVertex;
	case eHWSC_Pixel:
		return s_sClassNamePixel;
	case eHWSC_Geometry:
		return s_sClassNameGeometry;
	case eHWSC_Domain:
		return s_sClassNameDomain;
	case eHWSC_Hull:
		return s_sClassNameHull;
	default:
		CRY_ASSERT("WTF");
	case eHWSC_Compute:
		return s_sClassNameCompute;
	}
}

CCryNameTSCRC CHWShader::mfGetCacheClassName(EHWShaderClass eClass)
{
	static const auto s_sClassNameVertex = CCryNameTSCRC("CHWShader_cache_VS");
	static const auto s_sClassNamePixel = CCryNameTSCRC("CHWShader_cache_PS");
	static const auto s_sClassNameGeometry = CCryNameTSCRC("CHWShader_cache_GS");
	static const auto s_sClassNameDomain = CCryNameTSCRC("CHWShader_cache_DS");
	static const auto s_sClassNameHull = CCryNameTSCRC("CHWShader_cache_HS");
	static const auto s_sClassNameCompute = CCryNameTSCRC("CHWShader_cache_CS");

	switch (eClass)
	{
	case eHWSC_Vertex:
		return s_sClassNameVertex;
	case eHWSC_Pixel:
		return s_sClassNamePixel;
	case eHWSC_Geometry:
		return s_sClassNameGeometry;
	case eHWSC_Domain:
		return s_sClassNameDomain;
	case eHWSC_Hull:
		return s_sClassNameHull;
	default:
		CRY_ASSERT("WTF");
	case eHWSC_Compute:
		return s_sClassNameCompute;
	}
}


CHWShader* CHWShader::mfForName(const char* name, const char* nameSource, const char* szEntryFunc, EHWShaderClass eClass, CShader* pFX, uint64 nMaskGen, uint64 nMaskGenFX)
{
	//	CRY_PROFILE_FUNCTION(PROFILE_LOADING_ONLY)(iSystem);
	if (!name || !name[0])
		return nullptr;
	MEMSTAT_CONTEXT(EMemStatContextType::Shader, name);

	CHWShader_D3D* pSH	   = nullptr;
	stack_string   strName = name;
	stack_string   AddStr;
	const auto	   className = mfGetClassName(eClass);

	const auto	 cacheClassName = mfGetCacheClassName(eClass);
	const string cacheName		= strName;
	const auto	 cacheNameCrc	= CCryNameTSCRC{cacheName.c_str()};

	#if 0
	if (nMaskGen)
	{
#if defined(CRY_COMPILER_GCC) || defined(CRY_COMPILER_CLANG)
		strName += AddStr.Format("(%llx)", nMaskGen);
#else
		strName += AddStr.Format("(%I64x)", nMaskGen);
#endif
	}
	#endif
	const CCryNameTSCRC Name = strName.c_str();

	CBaseResource* pBR = CBaseResource::GetResource(className, Name, true);
	if (!pBR)
	{
		pSH					= new CHWShader_D3D;
		pSH->m_Name			= strName.c_str();
		pSH->m_NameSourceFX = nameSource;
		pSH->Register(className, Name);
		pSH->mfFree();

		pSH->m_EntryFunc = szEntryFunc;
		//pSH->m_CRC32	 = CRC32;
		pSH->m_eSHClass	 = eClass;
	}
	else
	{
		pSH = static_cast<CHWShader_D3D*>(pBR);
		#if 0
		if (pSH->m_CRC32 == CRC32)
		{
			if (CRenderer::CV_r_shadersAllowCompilation)
			{
				if (SHData.size())
				{
					pSH->mfStoreCacheTokenMap(Table, SHData);
				}
			}
			return pSH;
		}
		#endif

		// CRC mismatch
		pSH->mfFree();
		//pSH->m_CRC32	= CRC32;
		pSH->m_eSHClass = eClass;
	}

	// Acquire cache resource
	auto* hwSharedCache = CBaseResource::GetResource(cacheClassName, cacheNameCrc, true);
	#if 0
	if (!hwSharedCache)
	{
		char dstName[256];
		pSH->mfGetDstFileName(nullptr, dstName, 256, 0);

		hwSharedCache = new SHWShaderCache(string{dstName});
		hwSharedCache->Register(cacheClassName, cacheNameCrc);
	}
	pSH->m_pCache = static_cast<SHWShaderCache*>(hwSharedCache);

	if (CParserBin::m_bEditable || ((!pVRProjectionManager || pVRProjectionManager->IsMultiResEnabledStatic()) && eClass == eHWSC_Vertex))
	{
		pSH->m_TokenTable = Table;
		pSH->m_TokenData  = SHData;
	}
	#endif

	#if 0
	pSH->m_dwShaderType	  = dwType;
	pSH->m_nMaskGenShader = nMaskGen;
	pSH->m_nMaskGenFX	  = nMaskGenFX;

	pSH->mfWarmupCache(pFX);
	#endif

	#if 1
	#if 0
	pSH->mfConstructFX(Table, SHData);
	#endif
	#pragma message("TODO: Implement it!")
	#endif

	return pSH;
}

void CHWShader_D3D::mfReset()
{
}

CHWShader_D3D::~CHWShader_D3D()
{
	mfFree();
}

const char* CHWShader::mfProfileString(EHWShaderClass type)
{
	const char* szProfile = "Unknown";
	return type == eHWSC_Vertex ? "vs_4_0" : type == eHWSC_Geometry ? "gs_4_0"
																		: "ps_4_0";
	switch (type)
	{
	case eHWSC_Vertex:
		szProfile = "vs_5_0";
		break;
	case eHWSC_Pixel:
		szProfile = "ps_5_0";
		break;
	case eHWSC_Geometry:
		szProfile = "gs_5_0";
		break;
	case eHWSC_Domain:
		break;
	case eHWSC_Hull:
		break;
	case eHWSC_Compute:
		szProfile = "cs_5_0";
		break;
	default:
		assert(0);
		break;
	}
	return szProfile;
}

// Compile pixel/vertex shader for the current instance properties
bool CHWShader_D3D::mfActivate(CShader* pSH, uint32 nFlags)
{
	PROFILE_FRAME(Shader_HWShaderActivate);

	bool bResult = true;
	SHWSInstance* pInst = m_pCurInst;

	//mfLogShaderRequest(pInst);

	#if 0
	if (mfIsValid(pInst, true) == ED3DShError_NotCompiled)
	{
		char name[128];
		mfGenName(pInst, name, 128, 1);
	}
	#endif
	return false;
}

bool CHWShader_D3D::Upload(SHWSInstance* pInst, ID3DBlob* pBlob, CShader* pSH)
{
	HRESULT hr;
	auto	pBuf = (DWORD*)pBlob->GetBufferPointer();
	auto	nSize = pBlob->GetBufferSize();

	d3dShaderHandleType* handle{};

	switch (m_eSHClass)
	{
	case eHWSC_Vertex:
		hr = (handle = GetDeviceObjectFactory().CreateVertexShader(pBuf, nSize)) ? S_OK : E_FAIL;
		break;
	case eHWSC_Geometry:
		hr = (handle = GetDeviceObjectFactory().CreateGeometryShader(pBuf, nSize)) ? S_OK : E_FAIL;
		break;
	case eHWSC_Pixel:
		hr = (handle = GetDeviceObjectFactory().CreatePixelShader(pBuf, nSize)) ? S_OK : E_FAIL;
		break;
	case IShader::E_NUM:
		break;
	default:
		break;
	}

	if (SUCCEEDED(hr))
	{
		if (handle && hr == S_OK)
			pInst->m_Handle = SD3DShaderHandle(handle, m_eSHClass, nSize);
	// Assign name to Shader for enhanced debugging
#if !defined(RELEASE) && (BB_PLATFORM_WINDOWS)
	char name[1024];
	sprintf(name, "%s_%s", pSH->GetName(), m_EntryFunc.c_str());

#	if BB_PLATFORM_WINDOWS
#		if BB_RENDERER_DIRECT3D
	auto pObject = (ID3D11DeviceChild*)pInst->m_Handle.m_pShader->GetHandle();
	::SetDebugName(pObject, name);
#		elif BB_RENDERER_VULKAN
	auto pObject = reinterpret_cast<NCryVulkan::CShader*>(pInst->m_Handle.m_pShader->GetHandle());
	SetDebugName(pObject, name);
#		endif
#	endif
#endif
	}
	return (hr == S_OK);
}

void CHWShader_D3D::GetMemoryUsage(ICrySizer* pSizer) const
{
}

bool CHWShader_D3D::mfCompileHLSL_Int(CShader* pSH, char* prog_text, D3DBlob** ppShader, void** ppConstantTable, D3DBlob** ppErrorMsgs, string& strErr)
{
	HRESULT hr = S_OK;
	CHWShader_D3D::SHWSInstance* pInst = m_pCurInst;
	const char* szProfile = mfProfileString(pInst->m_eClass);
	const char* pFunCCryName = m_EntryFunc.c_str();

	bool		bRes = true;

	auto nFlags = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
	hr			= D3DCompile(prog_text, strlen(prog_text), GetName(), NULL, NULL, pFunCCryName, szProfile, nFlags, 0, (ID3DBlob**)ppShader, (ID3DBlob**)ppErrorMsgs);


	if (FAILED(hr) || !*ppShader)
	{
		if (ppErrorMsgs && *ppErrorMsgs)
		{
			const char* err = (const char*)ppErrorMsgs[0]->GetBufferPointer();
			strErr += err;
		}
		else
		{
			strErr += "D3DXCompileShader failed";
		}
		bRes = false;
	}
	else
	{
		void* pShaderReflBuf;
		UINT* pData = (UINT*)ppShader[0]->GetBufferPointer();
		UINT  nSize = (uint32)ppShader[0]->GetBufferSize();
		hr			= D3DReflection(pData, nSize, IID_D3DShaderReflection, &pShaderReflBuf);
		if (SUCCEEDED(hr))
		{
			D3DShaderReflection* pShaderReflection = (D3DShaderReflection*)pShaderReflBuf;
			*ppConstantTable					   = (void*)pShaderReflection;
		}
		else
		{
			assert(0);
		}
	}
	return bRes;
}

D3DBlob* CHWShader_D3D::mfCompileHLSL(CShader* pSH, char* prog_text, void** ppConstantTable, D3DBlob** ppErrorMsgs, uint32 nFlags)
{
	string strErr;
	D3DBlob* pCode = NULL;

	if (!prog_text)
	{
		assert(0);
		return NULL;
	}
	auto res = mfCompileHLSL_Int(pSH, prog_text, &pCode, ppConstantTable, ppErrorMsgs, strErr);

	return pCode;
}
