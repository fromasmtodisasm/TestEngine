#include "Effect.hpp"
#include "../D3D/Shader.hpp"

IShader* CEffect::GetShader(const char* name)
{
	return nullptr;
}

int CEffect::GetNumTechniques()
{
	return m_Techniques.size();
}

ITechnique* CEffect::GetTechnique(int i)
{
	assert(i >= 0 && i < m_Techniques.size());
	return &m_Techniques[i];
}

ITechnique* CEffect::GetTechnique(const char* name, size_t size)
{
	for (size_t i = 0; i < m_Techniques.size(); i++)
	{
		if (std::string_view(m_Techniques[i].Name) == std::string_view(name, size))
		{
			return &m_Techniques[i];
		}
	}
	return nullptr;
}

void CEffect::shader_assignment(EHWShaderClass type, const string& name)
{
	auto& tech			   = m_Techniques.back();
	auto& pass			   = tech.Passes.back();
	pass.EntryPoints[type] = name;
}

bool CEffect::SetLang(ShaderLangId id)
{
	if (m_LangId != ShaderLangId::None)
		return false;
	m_LangId = id;
	return true;
}

const char* CEffect::GetName()
{
	return m_name.data();
}

ShaderLangId CEffect::GetLangId()
{
	return m_LangId;
}

const char* CEffect::GetCode()
{
	return m_Code.data();
}

int CTechnique::GetNumPasses()
{
	return Passes.size();
}

bool CTechnique::CompilePass(int i)
{
	return false;
}

SShaderPass* CTechnique::GetPass(int i)
{
	return &Passes[i];
}

int CTechnique::GetId()
{
	return Id;
}

const char* CTechnique::GetName()
{
	return Name.data();
}
