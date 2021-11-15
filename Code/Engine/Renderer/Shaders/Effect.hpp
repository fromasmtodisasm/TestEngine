#pragma once
#include "FxParser.h"
#include <BlackBox/Renderer/IRender.hpp>
#include "../D3D/Shader.hpp"
#include <BlackBox/Renderer/VertexFormats.hpp>

class CTechnique : public ITechnique
{
  public:
	CTechnique(int id, std::string name)
		: Name(name)
		, Id(id)
	{
	}
	// Inherited via ITechnique
	virtual int			 GetNumPasses() override;
	virtual bool		 CompilePass(int i) override;
	virtual SShaderPass* GetPass(int i) override;
	virtual int			 GetId() override;
	virtual const char*	 GetName() override;
	;

	std::string Name;
	int			Id;
	//std::vector<std::string_view> CommonCode;
	std::vector<SShaderPass> Passes;
};

class CEffect : public IEffect
{
  public:
	CEffect(std::string_view name)
		: m_name(name)
	{
	}
	virtual IShader* GetShader(const char* name) override;

	virtual int			GetNumTechniques() override;
	virtual ITechnique* GetTechnique(int i) override;
	virtual ITechnique* GetTechnique(const char* name, size_t size) override;

	void shader_assignment(EHWShaderClass type, const string& name);
	bool SetLang(ShaderLangId id);

  public:
	std::string m_name;
	//std::vector<ShaderInfo> m_shaders;
	std::vector<CTechnique> m_Techniques;
	ShaderLangId			m_LangId = ShaderLangId::None;
	std::string				m_Code;

	// Inherited via IEffect
	virtual const char* GetName() override;

	// Inherited via IEffect
	virtual ShaderLangId GetLangId() override;

	// Inherited via IEffect
	virtual const char* GetCode() override;
};
