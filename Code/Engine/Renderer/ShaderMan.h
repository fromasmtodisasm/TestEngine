#pragma once
class CShader;
#include <thread>
#include <mutex>

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

	CShader* Reload(CShader* pShader);
	void     Update();

	void     ReloadAll();
	ShaderMan();
	~ShaderMan();

	std::mutex                        m_ReloadMutex;
	std::mutex                        m_ShadersAccessMutex;
	std::map<string, CShader*>        m_Shaders;
	std::vector<CShader*>             m_ToReload;
	std::thread::id                   m_nMainThreadId;

	std::function<void()>             m_Watch;
	std::array<std::set<CShader*>, 2> m_WatchedNames;
	int                               m_WatchIndex;

	std::thread                       m_ShaderWatcher;
	std::atomic<bool>                 m_bStopWatch;
};