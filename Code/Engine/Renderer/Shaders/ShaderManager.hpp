#pragma once

class CShaderManager
{
  public:
	void	 RT_ShaderLoad(const char* name, int flags, uint64 nMaskGen, CShader* p);
	CShader* NewShader()
	{
		return new CShader;
	}
	IShader* Sh_Load(const char* name, int flags, uint64 nMaskGen);
	bool Sh_LoadBinary(const char* name, int flags, uint64 nMaskGen, CShader* p) const
	{
		//return gEnv->pConsole->GetCVar("r_SkipShaderCache")->GetIVal() ? nullptr : CShader::LoadBinaryShader(name, flags, nMaskGen);
		return false;
	}

	bool Compile(std::string_view name, int flags, uint64 nMaskGen, CShader* p);

	bool LoadFromEffect(CShader* pSH, PEffect pEffect, int technique = 0, int pass = 0);
	bool LoadShader();
	void ReloadAll();

	std::map<string,_smart_ptr<CShader>> m_Shaders;
};

