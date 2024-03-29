#pragma once
#include <BlackBox/Core/Platform/Platform.hpp>
#include <BlackBox/Core/Utils.hpp>
#include <BlackBox/System/IConsole.hpp>
#include <BlackBox/System/ILog.hpp>
#include <BlackBox/System/ISystem.hpp>
#include <BlackBox/System/ConsoleRegistration.h>
#include <BlackBox/Threading/IThreadManager.h>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

template<class T, int = 32>
using StackStringT = std::string;

//////////////////////////////////////////////////////////////////////

#define MAX_FILENAME_SIZE 256

#if defined(DEDICATED_SERVER) && 0
	#define KEEP_LOG_FILE_OPEN 1
#else
	#define KEEP_LOG_FILE_OPEN 0
#endif

using CryCriticalSection = std::mutex;
namespace MT
{
	template<class T, class Alloc> class queue;
	template<class T> class vector;
}

namespace stl
{
	template<typename T> void free_container(MT::vector<T>& v);
	template<typename T, class Alloc> void free_container(MT::queue<T, Alloc>& v);
}

namespace MT
{

// Thread Safe wrappers on the standard STL containers.

//! Multi-Thread safe queue container, can be used instead of std::vector.
template<class T, class Alloc = std::allocator<T>>
class queue
{
public:
	typedef T                      value_type;
	typedef std::vector<T, Alloc>  container_type;
	using AutoLock =  std::lock_guard<std::mutex>;

	// std::queue interface
	const T& front() const           { AutoLock lock(m_cs); return v.front(); };
	const T& back() const            { AutoLock lock(m_cs);  return v.back(); }
	void     push(const T& x)        { AutoLock lock(m_cs); return v.push_back(x); };
	void     reserve(const size_t n) { AutoLock lock(m_cs); v.reserve(n); };
	// classic pop function of queue should not be used for thread safety, use try_pop instead
	//void	pop()							{ AutoLock lock(m_cs); return v.erase(v.begin()); };

	CryCriticalSection& get_lock() const { return m_cs; }

	bool                empty() const    { AutoLock lock(m_cs); return v.empty(); }
	int                 size() const     { AutoLock lock(m_cs); return v.size(); }
	void                clear()          { AutoLock lock(m_cs); v.clear(); }
	void                free_memory()    { AutoLock lock(m_cs); stl::free_container(v); }

	template<class Func>
	void sort(const Func& compare_less) { AutoLock lock(m_cs); std::sort(v.begin(), v.end(), compare_less); }

	//////////////////////////////////////////////////////////////////////////
	bool try_pop(T& returnValue)
	{
		AutoLock lock(m_cs);
		if (!v.empty())
		{
			returnValue = v.front();
			v.erase(v.begin());
			return true;
		}
		return false;
	};

	//////////////////////////////////////////////////////////////////////////
	bool try_remove(const T& value)
	{
		AutoLock lock(m_cs);
		if (!v.empty())
		{
			typename container_type::iterator it = std::find(v.begin(), v.end(), value);
			if (it != v.end())
			{
				v.erase(it);
				return true;
			}
		}
		return false;
	};

	template<typename Sizer>
	void GetMemoryUsage(Sizer* pSizer) const
	{
		pSizer->AddObject(v);
	}

	container_type pop_all()
	{
		container_type result;
		{
			AutoLock lock(m_cs);
			v.swap(result);
		}
		return result;
	}

private:
	container_type             v;
	mutable CryCriticalSection m_cs;
};

}

namespace stl
{
	template<typename T> void free_container(MT::vector<T>& v)
	{
		v.free_memory();
	}
	template<typename T, class Alloc> void free_container(MT::queue<T, Alloc>& v)
	{
		v.free_memory();
	}
}

class CLog : 
	public ILog
	, public ISystemEventListener
{
  public:
	typedef StackStringT<char, 256> LogStringType;
	CLog(ISystem* pSystem);
	~CLog();
	
  private:
	void Shutdown()
	{
	}

	virtual void Release() override;
	virtual void LogV(const ELogType nType, const char* szFormat, va_list args) override;
	virtual void LogToConsole(const char* command, ...) override;
	virtual void SetFileName(const char* command = NULL) override;
	virtual const char* GetFileName() override;
	virtual void LogToFile(const char* command, ...) override;
	virtual void LogV(IMiniLog::ELogType nType, int flags, const char* szFormat, va_list args);
	const char* LogTypeToString(IMiniLog::ELogType type);
	virtual void Log(const char* szCommand, ...) override;
	virtual void LogWarning(const char* szCommand, ...) override;
	virtual void LogError(const char* szCommand, ...) override;
	virtual void LogPlus(const char* command, ...) override;
	virtual void LogToFilePlus(const char* command, ...) override;
	virtual void LogToConsolePlus(const char* command, ...) override;
	virtual void UpdateLoadingScreen(const char* command, ...) override;
	virtual void UpdateLoadingScreenPlus(const char* command, ...) override;
	virtual void EnableVerbosity(bool bEnable) override;
	virtual void SetVerbosity(int verbosity) override;
	virtual int GetVerbosityLevel() override;

	void LogStringToFile(const char* szString, bool bAdd, bool bError = false);
	void LogStringToConsole(const char* szString, bool bAdd = false);

	FILE* OpenLogFile(const char* filename, const char* mode);
	void  CloseLogFile(bool force = false);

	ISystem*                  m_pSystem;
	float                     m_fLastLoadingUpdateTime; // for non-frequent streamingEngine update
	char                      m_szFilename[MAX_FILENAME_SIZE];      // can be with path
	string                    m_filePath;               // Contains the full absolute path to the log file
	string                    m_backupFilePath;         // Contains the full absolute path to the backup log file
	FILE*                     m_pLogFile = nullptr;
	StackStringT<char, 32> m_LogMode;                // mode m_pLogFile has been opened with
	FILE*                     m_pErrFile = nullptr;
	int                       m_nErrCount = 0;
	string                    m_logFormat;              // Time logging format

	ICVar*             m_pLogIncludeTime;                 //

	IConsole*          m_pConsole;                        //

	std::mutex m_logCriticalSection;
  private:
	// checks the verbosity of the message and returns NULL if the message must NOT be
	// logged, or the pointer to the part of the message that should be logged
	const char* CheckAgainstVerbosity(const char* pText, bool& logtofile, bool& logtoconsole, const uint8 DefaultVerbosity = 2);

	const char* filename = "log.txt";
	bool inited			 = false;
	std::vector<std::string> log;
	char buf[10*1024] = {0};


	ICVar* m_pLogVerbosity					   = nullptr; //
	ICVar* m_pLogWriteToFile				   = nullptr; //
	ICVar* m_pLogWriteToFileVerbosity		   = nullptr; //
	ICVar* m_pLogVerbosityOverridesWriteToFile = nullptr; //
	ICVar* m_pLogSpamDelay					   = nullptr; //
	ICVar* m_pLogModule						   = nullptr;

	std::thread::id  m_nMainThreadId;
	struct SLogMsg
	{
		LogStringType msg;
		bool          bError;
		bool          bAdd;
		bool          bConsole;
		void          GetMemoryUsage(ICrySizer* pSizer) const {}
	};
	MT::queue<SLogMsg>              m_threadSafeMsgQueue;

	// Inherited via ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// Module filter for log
#if KEEP_LOG_FILE_OPEN
	static void LogFlushFile(IConsoleCmdArgs* pArgs);

	bool m_bFirstLine = true;
	char m_logBuffer[256 * 1024];
#endif
};
