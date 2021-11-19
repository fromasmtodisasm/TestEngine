#pragma once
#include "../D3D/Shader.hpp"
#include "FxParser.h"
#include <BlackBox/Renderer/IRender.hpp>
#include <BlackBox/Renderer/VertexFormats.hpp>
class SShaderPass;
class CHWShader;
namespace nvFX
{
	/*************************************************************************/ /**
	** \brief IUniform Parameter interface
	**/
	/*************************************************************************/
	class IUniform
	{
	  protected:
		IUniform() {}

	  public:
		virtual ~IUniform() {}
		enum Type {
			TUndefined = 0,
			TInt, TInt2, TInt3, TInt4,
			//UInt,
			TBool, TBool2, TBool3, TBool4,
			TFloat, TVec2, TVec3, TVec4,
			TMat2, TMat2x4, TMat3, TMat34,TMat4, 
			TUBO, // Uniform Buffer Object
			TCB,  // Constant Buffer (D3D)
			TUniform,
			TTexture, // a generic texture : for example when the app created this uniform to bind a texture to a sampler
			// Note: texture==sampler in this type list. D3D uses texture objects, OpenGL will use samplers...
			// Those typed texture/GL-samplers are when the parser encoutered an external declaration of uniform : it needs to know the type
			TTexture1D,
			TTexture2D,
			TTexture2DShadow,
			TTexture2DRect,
			TTexture3D,
			TTextureCube,
			TTexUnit, //TODO: check about this... used for the Cg case (can't assign texunit)
			// TODO : add missing cases
		};
		enum PrecisionType /// precision is only used for OpenGL ES
		{
			PHighP	 = 0,
			PMediumP = 1,
			PLowP	 = 2,
		};
	};
	inline std::string toString(IUniform::Type type)
	{
		switch (type)
		{
		case nvFX::IUniform::TFloat:
			return "float";
		case nvFX::IUniform::TVec2:
			return "vec2";
		case nvFX::IUniform::TVec3:
			return "vec3";
		case nvFX::IUniform::TVec4:
			return "vec4";
		case nvFX::IUniform::TInt:
			return "int";
		case nvFX::IUniform::TInt2:
			return "ivec2";
		case nvFX::IUniform::TInt3:
			return "ivec3";
		case nvFX::IUniform::TInt4:
			return "ivec4";
		case nvFX::IUniform::TBool:
			return "bool";
		case nvFX::IUniform::TBool2:
			return "bvec2";
		case nvFX::IUniform::TBool3:
			return "bvec3";
		case nvFX::IUniform::TBool4:
			return "bvec4";
		case nvFX::IUniform::TMat2:
			return "mat2";
		case nvFX::IUniform::TMat3:
			return "mat3";
		case nvFX::IUniform::TMat4:
			return "mat4";
		default:
			assert(0);
			return "unknown_type";
		}
	}
} // namespace nvFX

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

	CHWShader* GetHWShader(EHWShaderClass type, SShaderPass& pass);

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
