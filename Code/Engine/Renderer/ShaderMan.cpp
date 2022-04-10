#include "Common/CommonRender.h"
#include "BaseRenderer.hpp"

#include "ShaderMan.h"
#include "D3D/Shader.hpp"
#include <Shaders/FxParser.h>

IShader* ShaderMan::Sh_Load(const char* name, int flags, uint64 nMaskGen)
{
	auto pShader = _smart_ptr<CShader>(NewShader());
	//CShader* pShader;
	gRenDev->ExecuteRenderThreadCommand([=,&pShader]
	                                    {
												CryLog("load shader: %s", name);
												RT_ShaderLoad(name, flags, nMaskGen, &pShader); });
	if (pShader->GetFlags2() & EF2_FAILED)
	{
		pShader = nullptr;
	}
	return pShader;
}

bool ShaderMan::Sh_LoadBinary(const char* name, int flags, uint64 nMaskGen, CShader* p) const
{
	//return Env::Console()->GetCVar("r_SkipShaderCache")->GetIVal() ? nullptr : CShader::LoadBinaryShader(name, flags, nMaskGen);
	return false;
}

bool ShaderMan::Compile(std::string_view name, int flags, uint64 nMaskGen, CShader* p)
{
	PEffect           pEffect = nullptr;
	std::stringstream path;
	auto              pos       = name.find_last_of('.');
	std::string_view  real_name = name;
	std::string_view  technique;
	int               pass = 0;
	if (pos != name.npos)
	{
		real_name = name.substr(0, pos);
		technique = name.substr(pos + 1);
	}
	path << real_name << ".fx";
	auto shader_path = PathUtil::Make(PathUtil::Make(PathUtil::GetEnginePath(), string("Engine/shaders/fx")), path.str());
	if (g_FxParser->Parse(shader_path.c_str(), &pEffect))
	{
		auto nTech = 0;
		if (auto tech = pEffect->GetTechnique(technique.data(), technique.length()); tech != nullptr)
			nTech = tech->GetId();
		p->m_NameShader = name;
		p->m_NameFile   = path.str();
		if (CShader::LoadFromEffect(p, pEffect, nTech, pass))
		{
			p->ReflectShader();
			p->CreateInputLayout();
			p->m_Flags = flags;
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

CShader* ShaderMan::Reload(CShader* pShader)
{
	string name = pShader->GetName();
	auto flags = pShader->GetFlags();

	auto it = m_Shaders.find(pShader->m_NameShader);
	m_Shaders.erase(it);
	/*if (pShader->m_NumRefs > 0)
	{
		pShader->~CShader();
	}*/

	auto shader = (CShader*)Sh_Load(name.c_str(), flags, 0);
	return shader;

}

void ShaderMan::ReloadAll()
{
	for (auto& s : m_Shaders)
	{
		s.second->Reload(0);
	}
}

ShaderMan::~ShaderMan()
{
	auto f = fopen("shader.txt", "wb");
	for (auto& [k, v] : m_Shaders)
	{
		fprintf(f, "Shader: %s has %d references\n", k.c_str(), v->m_NumRefs);
		auto s = v.ReleaseOwnership();
		s->Release(true);
	}
	fclose(f);
}

void ShaderMan::RT_ShaderLoad(const char* name, int flags, uint64 nMaskGen, _smart_ptr<CShader>* p)
{
	if (auto& it = m_Shaders.find((name)); it != m_Shaders.end())
	{
		CryLog("Shader <%s> already cached", name);
		//it->second->AddRef();
		auto s = (*p).ReleaseOwnership();
		s->Release();
		*p = it->second;
		return;
	}
	if (!Sh_LoadBinary(name, flags, nMaskGen, *p))
	{
		if (Compile(name, flags, nMaskGen, *p))
		{
			(*p)->SaveBinaryShader(name, flags, nMaskGen);
		}
		else
		{
			(*p)->Release();
			*p = nullptr;
		}
	}
}

