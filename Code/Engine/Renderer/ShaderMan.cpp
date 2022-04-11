#include "Common/CommonRender.h"
#include "BaseRenderer.hpp"

#include "ShaderMan.h"
#include "D3D/Shader.hpp"
#include <Shaders/FxParser.h>
#include <filesystem>

IShader* ShaderMan::Sh_Load(const char* name, int flags, uint64 nMaskGen)
{
	auto pShader = _smart_ptr<CShader>(NewShader());
	//CShader* pShader;
	gRenDev->ExecuteRenderThreadCommand([=, &pShader]
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
		p->m_NameFile   = shader_path;
		if (CShader::LoadFromEffect(p, pEffect, nTech, pass))
		{
			p->ReflectShader();
			p->CreateInputLayout();
			p->m_Flags = flags;

			std::filesystem::path sp(p->m_NameFile);
			auto                  access = std::filesystem::last_write_time(sp);
			p->m_LastAccesTime           = access;

			p->m_Flags2 |= EF2_LOADED;
			delete pEffect;
			m_Shaders[string(name)] = p;
			m_WatchedNames[1 - m_WatchIndex].insert(p);
			p->AddRef();
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

#define CryGetCurrentThreadId() std::this_thread::get_id()

CShader* ShaderMan::Reload(CShader* pShader)
{
	if (CryGetCurrentThreadId() != m_nMainThreadId)
	{
		//std::unique_lock lock(m_ReloadMutex);
		m_ToReload.push_back(pShader);
		return nullptr;
	}
	string name   = pShader->GetName();
	auto   flags  = pShader->GetFlags();

	//std::unique_lock lock(m_ShadersAccessMutex);
	//auto             it = m_Shaders.find(pShader->m_NameShader);
	//m_Shaders.erase(it);
	/*if (pShader->m_NumRefs > 0)
	{
		pShader->~CShader();
	}*/

	auto   shader = (CShader*)Sh_Load(name.c_str(), flags | EF_RELOAD, 0);
	return shader;
}

void ShaderMan::Update()
{
	std::unique_lock lock(m_ReloadMutex);
	for each (auto& s in m_ToReload)
	{
		s->Reload(0);
	}
	m_ToReload.clear();

	m_WatchIndex = Env::Renderer()->GetFrameID() % 2;
}

void ShaderMan::ReloadAll()
{
	for (auto& s : m_Shaders)
	{
		s.second->Reload(0);
	}
}

ShaderMan::ShaderMan()
{
	m_nMainThreadId = std::this_thread::get_id();
	m_Watch         = [this]()
	{

#if 1
		while (!m_bStopWatch)
#endif
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
			for (auto& shader : m_WatchedNames[m_WatchIndex])
			{
				//std::unique_lock lock(m_ShadersAccessMutex);
				std::unique_lock      lock(m_ReloadMutex);
				std::filesystem::path sp(shader->m_NameFile);
				auto                  access = std::filesystem::last_write_time(sp);
				if (shader->m_LastAccesTime < access)
				{
					m_ToReload.push_back(shader);
					//Reload(shader);
				}
			}
		}
		//CryLog("Stop watching");
	};
	m_WatchIndex = Env::Renderer()->GetFrameID() % 2;
#if 1
	m_ShaderWatcher = std::thread(m_Watch);
#endif
}

ShaderMan::~ShaderMan()
{
	auto f       = fopen("shader.txt", "wb");
	m_bStopWatch = true;
	m_ShaderWatcher.join();

	for (auto& [k, v] : m_Shaders)
	{
		fprintf(f, "Shader: %s has %d references\n", k.c_str(), v->m_NumRefs);
		//auto s = v.ReleaseOwnership();
		v->Release(true);
	}
	fclose(f);
}

void ShaderMan::RT_ShaderLoad(const char* name, int flags, uint64 nMaskGen, _smart_ptr<CShader>* p)
{
	if (flags & EF_RELOAD)
	{
	}
	auto shader = stl::find_in_map(m_Shaders, name, nullptr);
	if (shader && !(flags & EF_RELOAD))
	{
		CryLog("Shader <%s> already cached", name);
		shader->AddRef();
		auto s = (*p).ReleaseOwnership();
		s->Release();
		*p = shader;
		return;
	}
	else if (shader)
	{
		shader->Release();
	}
	if (!shader)
	{
		shader = *p;
	}
	else
	{
		*p = shader;
	}
	if (!Sh_LoadBinary(name, flags, nMaskGen, shader))
	{
		if (Compile(name, flags, nMaskGen, shader))
		{
			shader->SaveBinaryShader(name, flags, nMaskGen);
		}
		else
		{
			shader->Release();
			shader = nullptr;
		}
	}
}
