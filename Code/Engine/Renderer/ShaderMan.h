#pragma once
class CShader;

class ShaderMan
{
public:
	void     RT_ShaderLoad(const char* name, int flags, uint64 nMaskGen, _smart_ptr<CShader>* p);
	CShader* NewShader()
	{
		return DEBUG_NEW CShader;
	}
	IShader* Sh_Load(const char* name, int flags, uint64 nMaskGen);
	bool     Sh_LoadBinary(const char* name, int flags, uint64 nMaskGen, CShader* p) const;

	bool     Compile(std::string_view name, int flags, uint64 nMaskGen, CShader* p);

	CShader*     Reload(CShader* pShader);

	void     ReloadAll();
	~ShaderMan();

	std::map<string, _smart_ptr<CShader>> m_Shaders;
};