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
	for (auto s : m_Shaders)
	{
		if (s)
		{
			auto sh = s->m_D3DShader;
			SAFE_RELEASE(sh);
		}
	}
}

CShader& CShader::operator=(const CShader& src)
{
	//uint32 i;

	//mfFree();

	int	  Offs = (int)(INT_PTR) & (((CShader*)0)->m_NumRefs);
	byte* d	   = (byte*)this;
	byte* s	   = (byte*)&src;
	memcpy(&d[Offs], &s[Offs], sizeof(CShader) - Offs);

	m_NameShader	 = src.m_NameShader;
	m_NameFile		 = src.m_NameFile;
	//m_NameShaderICRC = src.m_NameShaderICRC;

	#if 0
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
	if (auto s = m_Shaders[E_VERTEX]->m_D3DShader; s) { d->VSSetShader((PVertexShader)s, nullptr, 0); }
	//if (auto s = m_Shaders[E_GEOMETRY]->m_D3DShader; s) { d->VSSetShader((PVertexShader)s); }
	if (auto s = m_Shaders[E_FRAGMENT]->m_D3DShader; s) { d->PSSetShader((PPixelShader)s, nullptr, 0); }

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

	auto								  pVSBuf = (ID3DBlob*)m_Shaders[IShader::Type::E_VERTEX]->m_ByteCode;
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
				format.add(SP_DESC.SemanticName, DynVertexFormat::UInt);
				t_InputElementDesc.Format = DXGI_FORMAT_R32_UINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				format.add(SP_DESC.SemanticName, DynVertexFormat::SInt);
				t_InputElementDesc.Format = DXGI_FORMAT_R32_SINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				format.add(SP_DESC.SemanticName, DynVertexFormat::Float);
				t_InputElementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			t_ByteOffset += 4;
		}
		else if (SP_DESC.Mask <= 3)
		{
			if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				format.add(SP_DESC.SemanticName, DynVertexFormat::UVec2);
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				format.add(SP_DESC.SemanticName, DynVertexFormat::SVec2);
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				format.add(SP_DESC.SemanticName, DynVertexFormat::FVec2);
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			t_ByteOffset += 8;
		}
		else if (SP_DESC.Mask <= 7)
		{
			if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				format.add(SP_DESC.SemanticName, DynVertexFormat::UVec3);
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				format.add(SP_DESC.SemanticName, DynVertexFormat::SVec3);
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				format.add(SP_DESC.SemanticName, DynVertexFormat::FVec3);
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
				format.add(SP_DESC.SemanticName, DynVertexFormat::UVec4);
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				format.add(SP_DESC.SemanticName, DynVertexFormat::SVec4);
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			}
			else if (SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				format.add(SP_DESC.SemanticName, DynVertexFormat::FVec4);
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

void CShader::LoadShader(SShaderPass* pass, EHWShaderClass st, std::vector<std::string>& code, CShader* pSH)
{
	auto entry = pass->EntryPoints[st].data();
	if (auto res = LoadFromMemory(code, st, entry); res.first)
	{
		auto shader		   = new CHWShader_D3D(st, entry);
		shader->m_ByteCode = res.first;
		if (shader->Upload(shader->m_ByteCode, pSH))
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
	return CShader::Load(code, type, pEntry, true);
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

const char* CShader::mfProfileString(EHWShaderClass type)
{
	const char* szProfile = "Unknown";
	return type == Type::E_VERTEX ? "vs_4_0" : type == Type::E_GEOMETRY ? "gs_4_0"
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

std::pair<ID3DBlob*,ID3DBlob*> CShader::Load(const std::string_view text, EHWShaderClass type, const char* pEntry, bool bFromMemory)
{
	HRESULT hr = S_OK;
	CHWShader_D3D::SHWSInstance* pInst = m_pCurInst;
	const char* szProfile = mfProfileString(pInst->m_eClass);
	const char* pFunCCryName = m_EntryFunc.c_str();

	ID3DBlob*	pShaderBlob{};
	ID3DBlob*	pErrorBlob{};
	const char* code = bFromMemory ? text.data() : nullptr;
	const char* file = bFromMemory ? nullptr : text.data();
	auto		size = text.size();

	auto nFlags = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
	auto hr		= D3DCompile(
		code,
		size,
		file,
		nullptr,
		nullptr,
		pEntry,
		szProfile,
		nFlags,
		0, //flags2
		&pShaderBlob,
		&pErrorBlob);
	if (FAILED(hr))
	{
		#if 0
		auto pBlob = _smart_ptr(pShaderBlob);
		auto error = _smart_ptr(pErrorBlob);
		if (pErrorBlob && pErrorBlob->GetBufferPointer())
		{
			auto log	  = std::string_view((const char*)pErrorBlob->GetBufferPointer());
			auto severity = VALIDATOR_WARNING;
			if (auto pos = log.find("warning"))
			{
				severity = VALIDATOR_WARNING;
			}
			else
			{
				severity = VALIDATOR_ERROR_DBGBRK;
			}
			CryWarning(VALIDATOR_MODULE_RENDERER, severity, "Error and warning from compilation:\n%s", log.data());
		}
		#endif
		return std::make_pair(nullptr,pErrorBlob);
	}
	else
	{
		#if 0
		void* pShaderReflBuf;
		UINT* pData = (UINT*)pShaderBlob->GetBufferPointer();
		UINT  nSize = (uint32)pShaderBlob->GetBufferSize();
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
		#endif
	}

	return std::make_pair(pShaderBlob,pErrorBlob);
}

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

bool CHWShader_D3D::Upload(ID3DBlob* pBlob, CShader* pSH)
{
	HRESULT hr;
	auto	pBuf = (DWORD*)pBlob->GetBufferPointer();
	auto	nSize = pBlob->GetBufferSize();

	IUnknown* handle{};

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
		m_D3DShader = (ID3D11Resource*)handle;
	// Assign name to Shader for enhanced debugging
#if !defined(RELEASE) && (BB_PLATFORM_WINDOWS)
	char name[1024];
	sprintf(name, "%s_%s", pSH->GetName(), m_EntryFunc.c_str());

#	if BB_PLATFORM_WINDOWS
#		if BB_RENDERER_DIRECT3D
	auto pObject = (ID3D11DeviceChild*)m_D3DShader;
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

CHWShader* CHWShader::mfForName(const char* name, const char* nameSource, const char* szEntryFunc, EHWShaderClass eClass, CShader* pFX, uint64 nMaskGen, uint64 nMaskGenFX)
{
	return nullptr;
}
