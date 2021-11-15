#include "../BaseRenderer.hpp"
#include "ShaderManager.hpp"

inline void CShaderManager::RT_ShaderLoad(const char* name, int flags, uint64 nMaskGen, CShader* p)
{
	if (auto it = m_Shaders.find((name)); it != m_Shaders.end())
	{
		CryLog("Shader <%s> already cached", name);
		it->second->AddRef();
		*p = *it->second;
		return;
	}
	if (!Sh_LoadBinary(name, flags, nMaskGen, p))
	{
		if (Compile(name, flags, nMaskGen, p))
		{
			p->SaveBinaryShader(name, flags, nMaskGen);
		}
	}
}

inline IShader* CShaderManager::Sh_Load(const char* name, int flags, uint64 nMaskGen)
{
	_smart_ptr<CShader> pShader = NewShader();
	gRenDev->ExecuteRenderThreadCommand([=]
										{
											CryLog("load shader: %s", name);
											RT_ShaderLoad(name, flags, nMaskGen, pShader);
										});
	return pShader;
}

inline bool CShaderManager::Compile(std::string_view name, int flags, uint64 nMaskGen, CShader* p)
{
	PEffect			  pEffect = nullptr;
	std::stringstream path;
	auto			  pos		= name.find_last_of('.');
	std::string_view  real_name = name;
	std::string_view  technique;
	int				  pass = 0;
	if (pos != name.npos)
	{
		real_name = name.substr(0, pos);
		technique = name.substr(pos + 1);
	}
	path << "Data/shaders/fx/" << real_name << ".fx";
	if (g_FxParser->Parse(path.str().data(), &pEffect))
	{
		auto nTech = 0;
		if (auto tech = pEffect->GetTechnique(technique.data(), technique.length()); tech != nullptr)
			nTech = tech->GetId();
		p->m_NameShader = real_name.data();
		p->m_NameFile	= path.str();
		if (LoadFromEffect(p, pEffect, nTech, pass))
		{
			p->AddRef();
			p->ReflectShader();
			p->CreateInputLayout();
			p->m_Flags2 |= EF2_LOADED;
			delete pEffect;
			m_Shaders[string(name)] = p;
			return true;
		}
		p->m_Flags2 |= EF2_FAILED;
		return false;
	}
	else
	{
		p->m_Flags2 |= EF2_FAILED;
	}
	return false;
}

bool CShaderManager::LoadFromEffect(CShader* pSH, PEffect pEffect, int nTechnique, int nPass)
{
	auto				tech = pEffect->GetTechnique(nTechnique);
	auto				pass = tech->GetPass(nPass);
	std::vector<string> code{pEffect->GetCode()};

	EHWShaderClass Types[] = {
		eHWSC_Vertex,
		eHWSC_Geometry,
		eHWSC_Pixel,
	};
	for (auto st : Types)
	{
		CHWShader::mfForName(pass, st, code, pSH);
	}
	return true;
}

inline void CShaderManager::ReloadAll()
{
	for (auto& s : m_Shaders)
	{
		s.second->Reload(0);
	}
}
