#include "System.hpp"
#include "Console.hpp"
#include "ConsoleBatchFile.h"

#include <BlackBox/Core/Utils.hpp>
#include <BlackBox/Input/IHardwareMouse.hpp>
#include <BlackBox/Input/IInput.hpp>
#include <BlackBox/Renderer/IFont.hpp>
#include <BlackBox/Renderer/IRender.hpp>
#include <BlackBox/Renderer/IRenderAuxGeom.hpp>
#include <BlackBox/ScriptSystem/IScriptSystem.hpp>
#include <BlackBox/System/IClipBoard.hpp>
#include <BlackBox/System/ILog.hpp>
#include <BlackBox/System/ConsoleRegistration.h>

//#include <BlackBox/Core/StringUtils.h>

#include "NullImplementation/NullFont.hpp"
#include "XConsoleVariable.h"
#include <BlackBox/Core/StringUtils.h>

#include <BlackBox/Core/Path.hpp>

#include <sstream>
#include <regex>

#define BACKGROUND_SERVER_CHAR '/'
#define CONST_TEMP_STRING(s)   s
//#define DEFENCE_CVAR_HASH_LOGGING

#ifndef BB_PLATFORM_WINAPI
//////////////////////////////////////////////////////////////////////////
int memicmp(LPCSTR s1, LPCSTR s2, DWORD len)
{
	int ret = 0;
	while (len--)
	{
		if ((ret = tolower(*s1) - tolower(*s2))) break;
		s1++;
		s2++;
	}
	return ret;
}
#endif

namespace ECVarTypeHelper
{
	template<typename T>
	constexpr const char* GetNameForT();
	template<>
	constexpr const char* GetNameForT<int>()
	{
		return "int";
	}
	template<>
	constexpr const char* GetNameForT<float>()
	{
		return "float";
	}
	template<>
	constexpr const char* GetNameForT<const char*>()
	{
		return "string";
	}

	inline const char* GetNameForECVar(ECVarType type)
	{
		switch (type)
		{
		case ECVarType::Invalid:
			return "?";
		case ECVarType::Int:
			return "int";
		case ECVarType::Float:
			return "float";
		case ECVarType::String:
			return "string";
		default:
			CRY_ASSERT(false);
			return "?";
		}
	}
} // namespace ECVarTypeHelper

static inline void AssertName(const char* szName)
{
#ifdef _DEBUG
	CRY_ASSERT(szName);

	// test for good console variable / command name
	const char* p          = szName;
	bool        bFirstChar = true;

	while (*p)
	{
		CRY_ASSERT((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9' && !bFirstChar) || *p == '_' || *p == '.');

		++p;
		bFirstChar = false;
	}
#endif
}

// user defined comparison - for nicer printout
inline int GetCharPrio(char x)
{
	if (x >= 'a' && x <= 'z')
		x += 'A' - 'a'; // make upper case

	if (x == '_')
		return 300;
	else
		return x;
}

// case sensitive
inline bool less_CVar_cstr(const char* left, const char* right)
{
	while (true)
	{
		const uint32 l = GetCharPrio(*left), r = GetCharPrio(*right);

		if (l < r)
			return true;
		if (l > r)
			return false;

		if (*left == 0 || *right == 0)
			break;

		++left;
		++right;
	}

	return false;
}

// case sensitive
inline bool less_CVar(const string& left, const string& right)
{
	size_t lt = 0, rt = 0;
	while (true)
	{
		const uint32 l = GetCharPrio(left[lt]), r = GetCharPrio(right[rt]);

		if (l < r)
			return true;
		if (l > r)
			return false;

		if (left[lt] == 0 || right[rt] == 0)
			break;

		++lt;
		++rt;
	}

	return false;
}

void Command_SetWaitSeconds(IConsoleCmdArgs* pCmd)
{
#if WAIT_SECONDS
	if (pCmd->GetArgCount() > 1)
	{
		CXConsole* pConsole = static_cast<CXConsole*>(Env::Console());
		if (pConsole->m_waitSeconds.GetValue() != 0)
			CryWarning(EValidatorModule::VALIDATOR_MODULE_SYSTEM, EValidatorSeverity::VALIDATOR_WARNING, "You are overwriting the current wait seconds!");
		pConsole->m_waitSeconds.SetSeconds(atof(pCmd->GetArg(1)));
		pConsole->m_waitSeconds += Env::Timer()->GetFrameStartTime();
	}
#endif
}

void Command_SetWaitFrames(IConsoleCmdArgs* pCmd)
{
	if (pCmd->GetArgCount() > 1)
	{
		CXConsole* pConsole = static_cast<CXConsole*>(Env::Console());
		if (pConsole->m_waitFrames != 0)
			CryWarning(EValidatorModule::VALIDATOR_MODULE_SYSTEM, EValidatorSeverity::VALIDATOR_WARNING, "You are overwriting the current wait frames!");
		pConsole->m_waitFrames = std::max(0, atoi(pCmd->GetArg(1)));
	}
}

void Command_Then(IConsoleCmdArgs* pCmd)
{
	if (pCmd->GetArgCount() > 1)
	{
		const char* szTmpStr = pCmd->GetCommandLine() + sizeof("then");
		Env::Console()->ExecuteString(szTmpStr, true, true);
	}
}

void ConsoleShow(IConsoleCmdArgs*)
{
	Env::Console()->ShowConsole(true);
}
void ConsoleHide(IConsoleCmdArgs*)
{
	Env::Console()->ShowConsole(false);
}

void Bind(IConsoleCmdArgs* cmdArgs)
{
	if (cmdArgs->GetArgCount() >= 3)
	{
		string arg;
		for (int i = 2; i < cmdArgs->GetArgCount(); ++i)
		{
			arg += cmdArgs->GetArg(i);
			arg += " ";
		}
		Env::Console()->CreateKeyBind(cmdArgs->GetArg(1), arg.c_str(), false);
	}
}

inline bool hasprefix(const char* s, const char* prefix)
{
	while (*prefix)
		if (*prefix++ != *s++) return false;
	return true;
}

#if !defined(_RELEASE) && !BB_PLATFORM_LINUX && !BB_PLATFORM_ANDROID && !BB_PLATFORM_APPLE
void Command_DumpCommandsVars(IConsoleCmdArgs* Cmd)
{
	const char* arg = "";

	if (Cmd->GetArgCount() > 1)
		arg = Cmd->GetArg(1);

	CXConsole* pConsole = static_cast<CXConsole*>(Env::Console());

	// txt
	pConsole->DumpCommandsVars(const_cast<char*>(arg));

	#if CRY_PLATFORM_WINDOWS
	// HTML
	CConsoleHelpGen Generator(*pConsole);
	Generator.Work();
	#endif
}

void Command_DumpVars(IConsoleCmdArgs* Cmd)
{
	#if 0
	bool includeCheat = false;

	if (Cmd->GetArgCount() > 1)
	{
		const char* arg = Cmd->GetArg(1);
		const int incCheat = atoi(arg);
		if (incCheat == 1)
			includeCheat = true;
	}

	CXConsole* pConsole = static_cast<CXConsole*>(Env::Console());
	pConsole->DumpVarsTxt(includeCheat);
	#endif
}
#endif

//////////////////////////////////////////////////////////////////////////
int            CXConsole::con_display_last_messages = 0;
int            CXConsole::con_line_buffer_size      = 5000;
float          CXConsole::con_font_size             = 28;
int            CXConsole::con_showonload            = 0;
int            CXConsole::con_debug                 = 0;
int            CXConsole::con_restricted            = 0;

#pragma region CXConsole
CXConsole::CXConsole(CSystem& system)
    : //m_managedConsoleCommandListeners(1)
    /*,*/ m_system(system)
{
	m_fRepeatTimer          = 0;
	m_pSysDeactivateConsole = 0;
	m_pFont                 = NULL;
	m_pRenderer             = NULL;
	m_pNetwork              = NULL; // EvenBalance - M. Quinn
	m_pInput                = NULL;
	m_pImage                = NULL;
	m_nCursorPos            = 0;
	m_nScrollPos            = 0;
	m_nScrollMax            = 250;
	m_nTempScrollMax        = m_nScrollMax;
	m_nScrollLine           = 0;
	m_nHistoryPos           = -1;
	m_nTabCount             = 0;
	m_bConsoleActive        = false;
	m_bActivationKeyEnable  = true;
	m_bIsProcessingGroup    = false;
	m_sdScrollDir           = sdNONE;
	m_bDrawCursor           = true;
	m_fCursorBlinkTimer     = 0;

	m_nCheatHashRangeFirst  = 0;
	m_nCheatHashRangeLast   = 0;
	m_bCheatHashDirty       = false;
	m_nCheatHash            = 0;

#if 0
	m_bStaticBackground = true;
#else
	m_bStaticBackground             = false;
#endif
	m_nProgress         = 0;
	m_nProgressRange    = 0;
	m_nLoadingBackTexID = 0;
	m_nWhiteTexID       = 0;

	m_deferredExecution = false;
	m_waitFrames        = 0;
#if WAIT_SECONDS
	m_waitSeconds = 0.0f;
#endif
	m_blockCounter          = 0;

	m_currentLoadConfigType = eLoadConfigDefault;
	m_readOnly              = false;

	m_bUseHistoryFile       = true;

#if 0
	CNotificationNetworkConsole::Initialize();
#endif
}

void CXConsole::OnConsoleCommand(const char* cmd)
{
	ExecuteString(cmd, false);
}

CXConsole::~CXConsole()
{
	if (Env::System())
		Env::System()->GetIRemoteConsole()->UnregisterListener(this);

#if 0
	CNotificationNetworkConsole::Shutdown();
#endif

	for (auto& nameValuePair : m_configVars)
	{
		if (nameValuePair.first == "sys_cvar_overrides_path") // from system.cfg, but not handled as a CVar
			continue;
		CryWarning(EValidatorModule::VALIDATOR_MODULE_SYSTEM, EValidatorSeverity::VALIDATOR_WARNING,
		           "The CVar '%s' was found in a config file but was never registered.", nameValuePair.first.c_str());
	}

	for (auto& nameArgumentsPair : m_configCommands)
	{
		CryWarning(EValidatorModule::VALIDATOR_MODULE_SYSTEM, EValidatorSeverity::VALIDATOR_WARNING,
		           "The command '%s %s' was found in a config file but was never registered.", nameArgumentsPair.first.c_str(), nameArgumentsPair.second.c_str());
	}

	while (!m_mapVariables.empty())
		UnregisterVariableImpl(m_mapVariables.begin());
	while (!m_mapCommands.empty())
	{
		string command = m_mapCommands.begin()->first;
		RemoveCommand(command.c_str());
	}
}

bool CXConsole::ParseCVarOverridesFile(const char* szSysCVarOverridesPathConfigFile)
{
#if defined(USE_RUNTIME_CVAR_OVERRIDES) && defined(__NOTDEFINED__)
	CRY_ASSERT(m_mapVariables.empty(), "There should not be any cvars registered before parsing the runtime CVar overrides file, num: %i", m_mapVariables.size());
	
	string sys_cvar_overrides_path;
	{
		CryPathString path;
		Env::CryPak()->AdjustFileName(szSysCVarOverridesPathConfigFile, path, 0);
		std::ifstream inFile;
		inFile.open(path);
		if (!inFile.is_open())
		{
			CRY_ASSERT(false, "Failed to open the system.cfg file containing sys_cvar_overrides_path: %s", path.c_str());
			return false;
		}
		std::stringstream strStream;
		strStream << inFile.rdbuf();
		std::string content = strStream.str();

		// remove commented out content ('--' or ';')
		std::regex removeCommentsRegex(R"((;.*[\r\n])|(--.*[\r\n]))");
		content = std::regex_replace(content, removeCommentsRegex, "");

		const std::regex parseCfgRegex(R"(^[ \t]*sys_cvar_overrides_path[ \t]*[=][ \t]*(.+).*)");
		std::smatch regexMatch;
		if (std::regex_search(content, regexMatch, parseCfgRegex) && regexMatch.size() == 2)
		{
			sys_cvar_overrides_path = regexMatch[1].str().c_str();
		}
		else
		{
			CRY_ASSERT(false, "Failed to find/parse sys_cvar_overrides_path in system.cfg: %s", path.c_str());
			return false;
		}
	}

	std::string content;
	{
		const string fullPath = PathUtil::IsRelativePath(sys_cvar_overrides_path.c_str()) 
			? PathUtil::Make(PathUtil::GetProjectFolder(), sys_cvar_overrides_path) 
			: sys_cvar_overrides_path;

		std::ifstream inFile;
		inFile.open(fullPath);
		if (!inFile.is_open())
		{
			CRY_ASSERT(false, "Failed to open the cvar overrides file in sys_cvar_overrides_path: %s", sys_cvar_overrides_path.c_str());
			return false;
		}
		std::stringstream strStream;
		strStream << inFile.rdbuf();
		content = strStream.str();
	}

	// remove commented out content
	std::regex removeCommentsRegex(R"((/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+/)|(//.*[\r\n]))");
	content = std::regex_replace(content, removeCommentsRegex, "");

	const std::regex parseOverridesRegex(R"(^(ADD_CVAR_OVERRIDE_NUMERIC|ADD_CVAR_OVERRIDE_STRING)\([\s\t]*(.+?)[\s\t]*\,[\s\t]*(.+?)[\s\t]*\).*)");
	std::smatch regexMatch;
	while (std::regex_search(content, regexMatch, parseOverridesRegex))
	{
		if (regexMatch.size() == 4)
		{
			string cvarName = regexMatch[2].str().c_str();
			cvarName = cvarName.Replace("\"", "");
			string cvarValStr = regexMatch[3].str().c_str();

			if (GetCVar(cvarName) != nullptr)
			{
				CRY_ASSERT(false, "Trying to override a CVar that was already registered: %s", cvarName.c_str());
				return false;
			}

			if (cvarValStr.find("\"") != string::npos)
			{
				// Check for beginning & ending quotes and then remove them
				const size_t firstQuotePos = cvarValStr.find_first_of("\"");
				const size_t lastQuotePos = cvarValStr.find_last_of("\"");
				if (firstQuotePos != string::npos && lastQuotePos != string::npos && lastQuotePos != firstQuotePos)
				{
					cvarValStr.erase(lastQuotePos, 1);
					cvarValStr.erase(firstQuotePos, 1);
					RegisterString(cvarName, cvarValStr, VF_COPYNAME | VF_CONST_CVAR);
				}
				else
				{
					CRY_ASSERT(false, "Error parsing runtime CVar override string: %s = %s", cvarName.c_str(), cvarValStr.c_str());
					return false;
				}
			}
			else if (cvarValStr.find(".") != string::npos)
			{
				char* end;
				const float val = std::strtof(cvarValStr.c_str(), &end);
				if (end == cvarValStr.end() || end == cvarValStr.end() - 1) // 'end' will be the f after the last decimal if it exists
				{
					RegisterFloat(cvarName, val, VF_COPYNAME | VF_CONST_CVAR);
				}
				else
				{
					CRY_ASSERT(false, "Failed to parse runtime CVar override float value: %s = %s", cvarName.c_str(), cvarValStr.c_str());
					return false;
				}
			}
			else
			{
				char* end;
				const int val = static_cast<int>(std::strtol(cvarValStr.c_str(), &end, 0));
				if (end == cvarValStr.end())
				{
					RegisterInt(cvarName.c_str(), val, VF_COPYNAME | VF_CONST_CVAR);
				}
				else
				{
					CRY_ASSERT(false, "Failed to parse runtime CVar override integer value: %s = %s", cvarName.c_str(), cvarValStr.c_str());
					return false;
				}
			}
		}
		else
		{
			CRY_ASSERT(false, "Error matching regex while parsing CVar overrides file: %s", sys_cvar_overrides_path);
			return false;
		}
		content = regexMatch.suffix();
	}
#endif // defined(USE_RUNTIME_CVAR_OVERRIDES)
	return true;
}
void CXConsole::PreProjectSystemInit()
{
#if !defined(_RELEASE) || defined(ENABLE_DEVELOPER_CONSOLE_IN_RELEASE)
	const int disableConsoleDefault = 0;
	const int disableConsoleFlags   = 0;
#else
	const int disableConsoleDefault = 1;
	const int disableConsoleFlags   = VF_CONST_CVAR | VF_READONLY;
#endif

	m_pSysDeactivateConsole = REGISTER_INT("sys_DeactivateConsole", disableConsoleDefault, disableConsoleFlags,
	                                       "0: normal console behavior\n"
	                                       "1: hide the console");

	REGISTER_INT("sys_cvar_logging", 1, VF_NULL,
	             "0: Disable CVar logging\n"
	             "1: Log attempts to set CVars to forbidden/out-of-range values\n"
	             "2: Log all CVar changes");

	REGISTER_CVAR(con_display_last_messages, 0, VF_NULL, ""); // keep default at 1, needed for gameplay
	REGISTER_CVAR(con_line_buffer_size, 1000, VF_NULL, "");
	REGISTER_CVAR(con_font_size, con_font_size, VF_NULL, "");
	REGISTER_CVAR(con_showonload, 0, VF_NULL, "Show console on level loading");
	REGISTER_CVAR(con_debug, 0, VF_CHEAT, "Log call stack on every GetCVar call");
	REGISTER_CVAR(con_restricted, con_restricted, VF_RESTRICTEDMODE, "0=normal mode / 1=restricted access to the console"); // later on VF_RESTRICTEDMODE should be removed (to 0)

	if (m_system.IsDevMode()    // unrestricted console for -DEVMODE
	    || Env::Get()->IsDedicated()) // unrestricted console for dedicated server
		con_restricted = 0;

		// test cases -----------------------------------------------

#if 0
	// cppcheck-suppress assertWithSideEffect
	assert(GetCVar("con_debug") != 0);                    // should be registered a few lines above
	// cppcheck-suppress assertWithSideEffect
	assert(GetCVar("Con_Debug") == GetCVar("con_debug")); // different case

	// editor
	// cppcheck-suppress assertWithSideEffect
	assert(strcmp(AutoComplete("con_"), "con_debug") == 0);
	// cppcheck-suppress assertWithSideEffect
	assert(strcmp(AutoComplete("CON_"), "con_debug") == 0);
	// cppcheck-suppress assertWithSideEffect
	assert(strcmp(AutoComplete("con_debug"), "con_display_last_messages") == 0);   // actually we should reconsider this behavior
	// cppcheck-suppress assertWithSideEffect
	assert(strcmp(AutoComplete("Con_Debug"), "con_display_last_messages") == 0);   // actually we should reconsider this behavior

	// game
	// cppcheck-suppress assertWithSideEffect
	assert(strcmp(ProcessCompletion("con_"), "con_debug ") == 0);
	ResetAutoCompletion();
	// cppcheck-suppress assertWithSideEffect
	assert(strcmp(ProcessCompletion("CON_"), "con_debug ") == 0);
	ResetAutoCompletion();
	// cppcheck-suppress assertWithSideEffect
	assert(strcmp(ProcessCompletion("con_debug"), "con_debug ") == 0);
	ResetAutoCompletion();
	// cppcheck-suppress assertWithSideEffect
	assert(strcmp(ProcessCompletion("Con_Debug"), "con_debug ") == 0);
#endif
	ResetAutoCompletion();
	m_sInputBuffer = "";

	REGISTER_COMMAND("ConsoleShow", &ConsoleShow, VF_NULL, "Opens the console");
	REGISTER_COMMAND("ConsoleHide", &ConsoleHide, VF_NULL, "Closes the console");

#if ALLOW_AUDIT_CVARS
	REGISTER_COMMAND("audit_cvars", &Command_AuditCVars, VF_NULL, "Logs all console commands and cvars");
#endif // ALLOW_AUDIT_CVARS

#if !defined(_RELEASE) && !(BB_PLATFORM_LINUX || BB_PLATFORM_ANDROID) && !BB_PLATFORM_APPLE
	REGISTER_COMMAND("DumpCommandsVars", &Command_DumpCommandsVars, VF_NULL,
	                 "This console command dumps all console variables and commands to disk\n"
	                 "DumpCommandsVars [prefix]");
	REGISTER_COMMAND("DumpVars", &Command_DumpVars, VF_NULL,
	                 "This console command dumps all console variables to disk\n"
	                 "DumpVars [IncludeCheatCvars]");
#endif

	REGISTER_COMMAND("Bind", &Bind, VF_NULL, "");
	REGISTER_COMMAND("wait_seconds", &Command_SetWaitSeconds, VF_BLOCKFRAME,
	                 "Forces the console to wait for a given number of seconds before the next deferred command is processed\n"
	                 "Works only in deferred command mode. See the 'then' command.\n"
	                 "The current value is replaced, use 'then wait_seconds X' for successive waits.");
	REGISTER_COMMAND("wait_frames", &Command_SetWaitFrames, VF_BLOCKFRAME,
	                 "Forces the console to wait for a given number of frames before the next deferred command is processed\n"
	                 "Works only in deferred command mode. See the 'then' command.\n"
	                 "The current value is replaced, use 'then wait_frames X' for successive waits.");
	REGISTER_COMMAND("then", &Command_Then, VF_NULL,
	                 "Causes a command to be deferred. That is, it will be executed after the time specified with 'wait_seconds' or 'wait_frames'. E.g:\n"
	                 "> wait_seconds 20\n"
	                 "> then echo Goodbye world!\n"
	                 "> then wait_seconds 5\n"
	                 "> then quit");
	CConsoleBatchFile::Init();
}

void CXConsole::PostRendererInit()
{
	m_pFont     = GetFont("VeraMono.ttf", con_font_size, con_font_size);
	m_pRenderer = m_system.GetIRenderer();
	m_pNetwork  = Env::Network(); // EvenBalance - M. Quinn
	m_pInput    = m_system.GetIInput();
	m_pTimer    = m_system.GetITimer();

	if (m_pInput)
		m_pInput->AddConsoleEventListener(this);

	if (m_pRenderer)
	{
		const char* texture_path = "textures/console/defaultconsole.dds";
		ICVar*      background   = GetCVar("console_background");
#if 0
		if (background != nullptr)
			texture_path = background->GetString();
#endif
		// This texture is already loaded by the renderer. It's ref counted so there is no wasted space.
		auto Tex      = m_system.GetIRenderer()->LoadTexture(texture_path, 0, 0);
		m_pImage      = m_system.GetIRenderer()->EF_GetTextureByID(Tex);
		m_nWhiteTexID = m_pImage ? m_pImage->GetTextureID() : 0;
	}
	else
	{
		m_nLoadingBackTexID = -1;
		m_nWhiteTexID       = -1;
	}

	if (Env::Get()->IsDedicated())
		m_bConsoleActive = true;

	if (con_showonload)
		ShowConsole(true);

	m_system.GetIRemoteConsole()->RegisterListener(this, "CXConsole");
}

void CXConsole::Copy()
{
#if BB_PLATFORM_WINDOWS
	if (m_sInputBuffer.empty() || !OpenClipboard(NULL))
		return;

	const size_t cbLength = m_sInputBuffer.length();
	#if 0
	const wstring textW = CryStringUtils::UTF8ToWStr(m_sInputBuffer);
	#endif
	const std::wstring textW    = str_to_wstr(m_sInputBuffer);
	const int          lengthA  = WideCharToMultiByte(CP_ACP, 0, textW.c_str(), -1, NULL, 0, NULL, NULL); //includes null terminator

	HGLOBAL            hGlobalW = GlobalAlloc(GHND, (textW.length() + 1) * sizeof(wchar_t));
	HGLOBAL            hGlobalA = GlobalAlloc(GHND, lengthA);
	LPVOID             pGlobalW = GlobalLock(hGlobalW);
	LPVOID             pGlobalA = GlobalLock(hGlobalA);

	wcscpy((wchar_t*)pGlobalW, textW.c_str());
	WideCharToMultiByte(CP_ACP, 0, textW.c_str(), -1, (LPSTR)pGlobalA, lengthA, NULL, NULL);

	GlobalUnlock(hGlobalW);
	GlobalUnlock(hGlobalA);

	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hGlobalW);
	SetClipboardData(CF_TEXT, hGlobalA);
	CloseClipboard();
#endif
}

void CXConsole::Paste()
{
	// FIX: This is ugly hack, event twice for frame sended
	static auto last_frame = 0;
	if (last_frame == Env::Renderer()->GetFrameID())
		return;
	last_frame = Env::Renderer()->GetFrameID();
#if BB_PLATFORM_WINDOWS
	const BOOL hasANSI    = IsClipboardFormatAvailable(CF_TEXT);
	const BOOL hasUnicode = IsClipboardFormatAvailable(CF_UNICODETEXT);

	if (!(hasANSI || hasUnicode))
		return;
	if (!OpenClipboard(NULL))
		return;

	HGLOBAL const hGlobal = GetClipboardData(hasUnicode ? CF_UNICODETEXT : CF_TEXT);
	if (!hGlobal)
	{
		CloseClipboard();
		return;
	}

	const void* const pGlobal = GlobalLock(hGlobal);
	if (!pGlobal)
	{
		CloseClipboard();
		return;
	}

	string temp;
	#if 0
	if (hasUnicode)
		temp = CryStringUtils::WStrToUTF8((const wchar_t*)pGlobal);
	else
		temp = CryStringUtils::ANSIToUTF8((const char*)pGlobal);
	#else
	temp = wstr_to_str((const wchar_t*)pGlobal);
	#endif

	GlobalUnlock(hGlobal);
	CloseClipboard();

	const size_t length = temp.length();
	m_sInputBuffer.insert(m_nCursorPos, temp /*, length*/);
	m_nCursorPos += length;
#endif
}
bool CXConsole::Init(ISystem* pSystem)
{
	return true;
}

void CXConsole::Release()
{
	delete this;
}

ICVar* CXConsole::CreateVariable(const char* sName, const char* sValue, int nFlags, const char* help /* = ""*/)
{
	AssertName(sName);

	ICVar* pCVar = stl::find_in_map(m_mapVariables, sName, nullptr);
	if (pCVar)
	{
		Env::Log()->LogError("[CVARS]: [DUPLICATE] CXConsole::RegisterString(const char*): variable [%s] is already registered", pCVar->GetName());
#if LOG_CVAR_INFRACTIONS_CALLSTACK
		Env::System()->debug_LogCallStack();
#endif // LOG_CVAR_INFRACTIONS_CALLSTACK
		return pCVar;
	}

	const string name(sName);
	pCVar = new CXConsoleVariableString(this, name, sValue, nFlags, help, true);
	RegisterVar(name, pCVar /*, pChangeFunc*/);
	return pCVar;
}

ICVar* CXConsole::CreateVariable(const char* sName, int iValue, int nFlags, const char* help /* = ""*/)
{
	AssertName(sName);

	ICVar* pCVar = stl::find_in_map(m_mapVariables, sName, nullptr);
	if (pCVar)
	{
		Env::Log()->LogError("[CVARS]: [DUPLICATE] CXConsole::RegisterInt(): variable [%s] is already registered", pCVar->GetName());
#if LOG_CVAR_INFRACTIONS_CALLSTACK
		Env::System()->debug_LogCallStack();
#endif // LOG_CVAR_INFRACTIONS_CALLSTACK
		return pCVar;
	}

	const string name(sName);
	pCVar = new CXConsoleVariableInt(this, name.data(), iValue, nFlags, help, true);
	RegisterVar(name, pCVar /*, pChangeFunc*/);
	return pCVar;
}

ICVar* CXConsole::CreateVariable(const char* sName, float fValue, int nFlags, const char* help /* = ""*/)
{
	AssertName(sName);

	ICVar* pCVar = stl::find_in_map(m_mapVariables, sName, nullptr);
	if (pCVar)
	{
		Env::Log()->LogError("[CVARS]: [DUPLICATE] CXConsole::RegisterFloat(): variable [%s] is already registered", pCVar->GetName());
#if LOG_CVAR_INFRACTIONS_CALLSTACK
		Env::System()->debug_LogCallStack();
#endif // LOG_CVAR_INFRACTIONS_CALLSTACK
		return pCVar;
	}

	const string name(sName);
	pCVar = new CXConsoleVariableFloat(this, name.data(), fValue, nFlags, help, true);
	RegisterVar(name, pCVar /*, pChangeFunc*/);
	return pCVar;
}

void CXConsole::UnregisterVariable(const char* sVarName, bool bDelete)
{
	auto iter = m_mapVariables.find(sVarName);
	if (iter == m_mapVariables.end())
		return;

	UnregisterVariableImpl(iter);
}

void CXConsole::AddCheckedCVar(ConsoleVariablesVector& vector, const ConsoleVariablesVector::value_type& value)
{
	ConsoleVariablesVector::iterator it = std::lower_bound(vector.begin(), vector.end(), value, CVarNameLess);

	if ((it == vector.end()) || strcmp(it->first, value.first))
		vector.insert(it, value);
}

void CXConsole::RemoveCheckedCVar(ConsoleVariablesVector& vector, const ConsoleVariablesVector::value_type& value)
{
	auto it = std::lower_bound(vector.begin(), vector.end(), value, CVarNameLess);
	if ((it != vector.end()) && !strcmp(it->first, value.first))
		vector.erase(it);
}

bool CXConsole::CVarNameLess(const std::pair<const char*, ICVar*>& lhs, const std::pair<const char*, ICVar*>& rhs)
{
	return strcmp(lhs.first, rhs.first) < 0;
}

void CXConsole::UnregisterVariableImpl(const ConsoleVariablesMap::iterator& iter)
{
	//UnRegisterAutoComplete(iter->first);

	ICVar*      pCVar = iter->second;
	const int32 flags = pCVar->GetFlags();
	if (flags & VF_CHEAT_ALWAYS_CHECK)
		RemoveCheckedCVar(m_alwaysCheckedVariables, std::make_pair(iter->first.data(), iter->second));
	else if ((flags & (VF_CHEAT | VF_CHEAT_NOCHECK)) == VF_CHEAT)
		RemoveCheckedCVar(m_randomCheckedVariables, std::make_pair(iter->first.data(), iter->second));

	m_mapVariables.erase(iter);

	for (auto& it : m_consoleVarSinks)
		it->OnVarUnregister(pCVar);

	if (pCVar->IsOwnedByConsole())
		delete pCVar;
}

void CXConsole::RegisterVar(const string& name, ICVar* pCVar, ConsoleVarFunc pChangeFunc)
{
	const bool isConst      = pCVar->GetFlags() & VF_CONST_CVAR != 0;
	const bool isCheat      = ((pCVar->GetFlags() & (VF_CHEAT | VF_CHEAT_NOCHECK | VF_CHEAT_ALWAYS_CHECK)) != 0);
	const bool isReadOnly   = ((pCVar->GetFlags() & VF_READONLY) != 0);
	const bool isDeprecated = ((pCVar->GetFlags() & VF_DEPRECATED) != 0);

	auto       it           = m_configVars.find(pCVar->GetName());
	if (it != m_configVars.end())
	{
		SConfigVar& var                = it->second;
		bool        allowChange        = true;
		const bool  wasProcessingGroup = GetIsProcessingGroup();
		SetProcessingGroup(var.m_partOfGroup);

		if (
#if CVAR_GROUPS_ARE_PRIVILEGED
		    !m_bIsProcessingGroup &&
#endif // !CVAR_GROUPS_ARE_PRIVILEGED
		    (isConst || isCheat || isReadOnly || isDeprecated))
		{
			allowChange = !isDeprecated && ((Env::System()->IsDevMode()) || (Env::Get()->IsEditor()));
			if (pCVar->GetString() != var.m_value && !allowChange)
			{
#if LOG_CVAR_INFRACTIONS
				LogChangeMessage(pCVar->GetName(), isConst, isCheat,
				                 isReadOnly, isDeprecated, pCVar->GetString(), var.m_value.c_str(), m_bIsProcessingGroup, allowChange);
	#if LOG_CVAR_INFRACTIONS_CALLSTACK
				Env::System()->debug_LogCallStack();
	#endif // LOG_CVAR_INFRACTIONS_CALLSTACK
#endif     // LOG_CVAR_INFRACTIONS
			}
		}

		if (allowChange)
		{
			pCVar->SetFromString(var.m_value.c_str());
			pCVar->SetFlags(pCVar->GetFlags() | var.nCVarOrFlags);
		}

		SetProcessingGroup(wasProcessingGroup);
		m_configVars.erase(it);
	}
	else
	{
		// Variable is not modified when just registered.
		pCVar->ClearFlags(VF_MODIFIED);
	}

#if 0
	if (pChangeFunc)
		pCVar->AddOnChange(pChangeFunc);
#endif

	auto pair = std::make_pair(name.data(), pCVar);
	m_mapVariables.insert(pair);

	const int flags = pCVar->GetFlags();

	if (flags & VF_CHEAT_ALWAYS_CHECK)
		AddCheckedCVar(m_alwaysCheckedVariables, pair);
	else if ((flags & (VF_CHEAT | VF_CHEAT_NOCHECK)) == VF_CHEAT)
		AddCheckedCVar(m_randomCheckedVariables, pair);
}

void CXConsole::RemoveCommand(const char* sName)
{
	m_mapCommands.erase(sName);
	UnRegisterAutoComplete(sName);
}

void CXConsole::SetInputLine(const char* szLine)
{
	CRY_ASSERT(szLine);
	m_sInputBuffer = szLine;
	m_nCursorPos   = m_sInputBuffer.size();
}

bool CXConsole::OnBeforeVarChange(ICVar* pVar, const char* sNewValue)
{
	const bool isConst      = pVar->IsConstCVar();
	const bool isCheat      = ((pVar->GetFlags() & (VF_CHEAT | VF_CHEAT_NOCHECK | VF_CHEAT_ALWAYS_CHECK)) != 0);
	const bool isReadOnly   = ((pVar->GetFlags() & VF_READONLY) != 0);
	const bool isDeprecated = ((pVar->GetFlags() & VF_DEPRECATED) != 0);

	if (
#if CVAR_GROUPS_ARE_PRIVILEGED
	    !m_bIsProcessingGroup &&
#endif
	    (isConst || isCheat || isReadOnly || isDeprecated))
	{
		const bool allowChange = !isDeprecated && ((Env::System()->IsDevMode()) || (Env::Get()->IsEditor()));
		if (!(Env::Get()->IsEditor()) || isDeprecated)
		{
#if LOG_CVAR_INFRACTIONS
			LogChangeMessage(pVar->GetName(), isConst, isCheat,
			                 isReadOnly, isDeprecated, pVar->GetString(), sNewValue, m_bIsProcessingGroup, allowChange);
	#if LOG_CVAR_INFRACTIONS_CALLSTACK
			Env::System()->debug_LogCallStack();
	#endif
#endif
		}

		if (!allowChange)
			return false;
	}

	for (IConsoleVarSink* sink : m_consoleVarSinks)
	{
		if (!sink->OnBeforeVarChange(pVar, sNewValue))
			return false;
	}
	return true;
}

void CXConsole::OnAfterVarChange(ICVar* pVar)
{
}

void CXConsole::OnVarUnregister(ICVar* pVar)
{
}

void CXConsole::SetScrollMax(int value)
{
	m_nScrollMax     = value;
	m_nTempScrollMax = m_nScrollMax;
}

void CXConsole::AddOutputPrintSink(IOutputPrintSink* inpSink)
{
	CRY_ASSERT(inpSink);
	m_OutputSinks.push_back(inpSink);
}

void CXConsole::RemoveOutputPrintSink(IOutputPrintSink* inpSink)
{
	CRY_ASSERT(inpSink);

	const int nCount = m_OutputSinks.size();
	for (int i = 0; i < nCount; i++)
	{
		if (m_OutputSinks[i] == inpSink)
		{
			if (nCount <= 1)
			{
				m_OutputSinks.clear();
			}
			else
			{
				m_OutputSinks[i] = m_OutputSinks.back();
				m_OutputSinks.pop_back();
			}
			return;
		}
	}
	CRY_ASSERT(false);
}

void CXConsole::ShowConsole(bool show)
{
	if (m_pSysDeactivateConsole->GetIVal())
		show = false;

	if (show && !m_bConsoleActive)
	{
		if (Env::HardwareMouse())
			Env::HardwareMouse()->IncrementCounter();
	}
	else if (!show && m_bConsoleActive)
	{
		if (Env::HardwareMouse())
			Env::HardwareMouse()->DecrementCounter();
	}

	SetStatus(show);

#if 0
	if (iRequestScrollMax > 0)
		m_nTempScrollMax = iRequestScrollMax;     // temporary user request
	else
#endif
	m_nTempScrollMax = m_nScrollMax; // reset

	if (m_bConsoleActive)
		m_sdScrollDir = sdDOWN;
	else
		m_sdScrollDir = sdUP;
}

int CXConsole::Register(const char* name, void* src, float defaultvalue, int flags, int type, const char* help /* = ""*/)
{
	//RegisterInternal(name, src, defaultvalue, flags, help);
	return 0;
}

float CXConsole::Register(const char* name, float* src, float defaultvalue, int flags /* = 0*/, const char* help /* = ""*/)
{
	RegisterInternal(name, src, defaultvalue, flags, help);
	return defaultvalue;
}

int CXConsole::Register(const char* name, int* src, float defaultvalue, int flags /* = 0*/, const char* help /* = ""*/)
{
	RegisterInternal(name, src, static_cast<int>(defaultvalue), flags, help);
	return static_cast<int>(defaultvalue);
}

ICVar* CXConsole::RegisterCVarGroup(const char* szName, const char* szFileName)
{
	AssertName(szName);
	assert(szFileName);

	// suppress cvars not starting with sys_spec_ as
	// cheaters might create cvars before we created ours
	if (strnicmp(szName, "sys_spec_", 9) != 0)
		return 0;

	ICVar* pCVar = stl::find_in_map(m_mapVariables, szName, NULL);
	if (pCVar)
		return pCVar; // Already registered, this is expected when loading engine specs after game specs.

	#if 0
	const string name(szName);
	CXConsoleVariableCVarGroup* pCVarGroup = new CXConsoleVariableCVarGroup(this, name, szFileName, VF_COPYNAME);

	pCVar = pCVarGroup;

	RegisterVar(name, pCVar, CXConsoleVariableCVarGroup::OnCVarChangeFunc);

	return pCVar;
	#else
	return nullptr;
	#endif
}


ICVar* CXConsole::RegisterInternal(const char* sName, int* src, int iValue, int nFlags, const char* help, ConsoleVarFunc pChangeFunc, bool allowModify)
{
	AssertName(sName);

	ICVar* pCVar = stl::find_in_map(m_mapVariables, sName, nullptr);
	if (pCVar)
	{
		if (pCVar->GetFlags() & VF_CONST_CVAR)
		{
			*src = pCVar->GetIVal();
		}
		else
		{
			Env::Log()->LogError("[CVARS]: [DUPLICATE] CXConsole::Register(int): variable [%s] is already registered", pCVar->GetName());
#if LOG_CVAR_INFRACTIONS_CALLSTACK
			Env::System()->debug_LogCallStack();
#endif // LOG_CVAR_INFRACTIONS_CALLSTACK
		}
		return pCVar;
	}

	if (!allowModify)
		nFlags |= VF_CONST_CVAR;
	*src = iValue; // Needs to be done before creating the CVar due to default overriding
	const string name(sName);
	pCVar = new CXConsoleVariableIntRef(this, name.data(), *src, nFlags, help, true);
	RegisterVar(name, pCVar, pChangeFunc);
	return pCVar;
}

#if 0
ICVar* CXConsole::RegisterCVarGroup(const char* szName, const char* szFileName)
{
	AssertName(szName);
	assert(szFileName);

	// suppress cvars not starting with sys_spec_ as
	// cheaters might create cvars before we created ours
	if (strnicmp(szName, "sys_spec_", 9) != 0)
		return 0;

	ICVar* pCVar = stl::find_in_map(m_mapVariables, szName, NULL);
	if (pCVar)
		return pCVar; // Already registered, this is expected when loading engine specs after game specs.

	const string name(szName);
	CXConsoleVariableCVarGroup* pCVarGroup = new CXConsoleVariableCVarGroup(this, name, szFileName, VF_COPYNAME);

	pCVar = pCVarGroup;

	RegisterVar(name, pCVar, CXConsoleVariableCVarGroup::OnCVarChangeFunc);

	return pCVar;
}
#endif
ICVar* CXConsole::RegisterInternal(const char* sName, float* src, float fValue, int nFlags, const char* help, ConsoleVarFunc pChangeFunc, bool allowModify)
{
	AssertName(sName);

	ICVar* pCVar = stl::find_in_map(m_mapVariables, sName, nullptr);
	if (pCVar)
	{
		if (pCVar->GetFlags() & VF_CONST_CVAR)
		{
			*src = pCVar->GetFVal();
		}
		else
		{
			Env::Log()->LogError("[CVARS]: [DUPLICATE] CXConsole::Register(float): variable [%s] is already registered", pCVar->GetName());
#if LOG_CVAR_INFRACTIONS_CALLSTACK
			Env::System()->debug_LogCallStack();
#endif // LOG_CVAR_INFRACTIONS_CALLSTACK
		}
		return pCVar;
	}
	if (!allowModify)
		nFlags |= VF_CONST_CVAR;
	*src = fValue; // Needs to be done before creating the CVar due to default overriding
	const string name(sName);
	pCVar = new CXConsoleVariableFloatRef(this, name.data(), *src, nFlags, help, true);
	RegisterVar(name, pCVar, pChangeFunc);
	return pCVar;
}

ICVar* CXConsole::RegisterInternal(const char* sName, const char** src, const char* defaultValue, int nFlags, const char* help, ConsoleVarFunc pChangeFunc, bool allowModify)
{
	AssertName(sName);

	ICVar* pCVar = stl::find_in_map(m_mapVariables, sName, nullptr);
	if (pCVar)
	{
		if (pCVar->GetFlags() & VF_CONST_CVAR)
		{
			*src = pCVar->GetString();
		}
		else
		{
			Env::Log()->LogError("[CVARS]: [DUPLICATE] CXConsole::Register(const char*): variable [%s] is already registered", pCVar->GetName());
#if LOG_CVAR_INFRACTIONS_CALLSTACK
			Env::System()->debug_LogCallStack();
#endif // LOG_CVAR_INFRACTIONS_CALLSTACK
		}
		return pCVar;
	}
	if (!allowModify)
		nFlags |= VF_CONST_CVAR;
	const string name(sName);
	//FIXME:
	pCVar = new CXConsoleVariableStringRef(this, name, *src, defaultValue, nFlags, help, true);
	RegisterVar(name, pCVar, pChangeFunc);
	return pCVar;
}

void CXConsole::DumpCVars(ICVarDumpSink* pCallback, unsigned int nFlagsFilter /* = 0*/)
{
	for (auto& pair : m_mapVariables)
	{
		if ((nFlagsFilter == 0) || ((nFlagsFilter != 0) && (pair.second->GetFlags() & nFlagsFilter)))
			pCallback->OnElementFound(pair.second);
	}
}

void CXConsole::CreateKeyBind(const char* sCmd, const char* sRes, bool bExecute)
{
	m_mapBinds.insert({sRes, sCmd});
}

void CXConsole::SetImage(ITexPic* pImage, bool bDeleteCurrent)
{
	if (bDeleteCurrent)
		pImage->Release();

	m_pImage = pImage;
}

ITexPic* CXConsole::GetImage()
{
	return nullptr;
}

void CXConsole::StaticBackground(bool bStatic)
{
	m_bStaticBackground = bStatic;
}

void CXConsole::SetLoadingImage(const char* szFilename)
{
	auto pTex = m_system.GetIRenderer()->LoadTexture(szFilename, 0,0);
	if (!pTex/* || (pTex->GetFlags() & FT_FAILED)*/)
	{
		auto t = Env::Renderer()->EF_GetTextureByID(pTex);
		SAFE_RELEASE(t);
		pTex = m_system.GetIRenderer()->LoadTexture("Textures/Console/loadscreen_default.dds", 0,0);
	}

	if (pTex)
		m_nLoadingBackTexID = pTex;
	else
		m_nLoadingBackTexID = -1;
}

bool CXConsole::GetLineNo(const DWORD indwLineNo, char* outszBuffer, const DWORD indwBufferSize) const
{
	assert(outszBuffer);
	assert(indwBufferSize > 0);

	outszBuffer[0] = 0;
	if (indwLineNo >= (int)m_dqConsoleBuffer.size())
		return false;

	auto ritor = m_dqConsoleBuffer.rbegin();
	ritor += indwLineNo;
	const char* buf = ritor->c_str();
	if (*buf > 0 && *buf < 32)
		buf++; // to jump over verbosity level character

#if 0
	strcpy(outszBuffer, indwBufferSize, buf);
#endif
	return true;
}

int CXConsole::GetLineCount() const
{
	return m_dqConsoleBuffer.size();
}

ICVar* CXConsole::GetCVar(const char* sName, const bool bCaseSensitive /* = true*/)
{
	CRY_ASSERT(sName, "Calling GetCVar with a nullptr");

	if (con_debug)
	{
		CryLog("GetCVar(\"%s\") called", sName);
#if 0
		m_system.debug_LogCallStack();
#endif
	}

	auto it = m_mapVariables.find(sName);
	if (it != m_mapVariables.end())
		return it->second;

	return nullptr;
	return nullptr;
}

CXFont* CXConsole::GetFont()
{
	return nullptr;
}

void CXConsole::Help(const char* command /* = NULL*/)
{
}

char* CXConsole::GetVariable(const char* szVarName, const char* szFileName, const char* def_val)
{
	return nullptr;
}

float CXConsole::GetVariable(const char* szVarName, const char* szFileName, float def_val)
{
	return 0.0f;
}

void CXConsole::PrintLine(const char* s)
{
	AddLine(s);
}

void CXConsole::PrintLinePlus(const char* s)
{
	AddLinePlus(s);
}

bool CXConsole::GetStatus()
{
	return m_bConsoleActive;
	return false;
}

void CXConsole::Clear()
{
	return;
	m_dqConsoleBuffer.clear();
}

void CXConsole::Update()
{
	// Execute the deferred commands
	ExecuteDeferredCommands();

	m_pRenderer = m_system.GetIRenderer();

	if (!m_bConsoleActive)
		m_nRepeatEvent.keyId = eKI_Unknown;

	// Process Key press repeat (backspace and cursor on PC)
	if (m_nRepeatEvent.keyId != eKI_Unknown)
	{
		const float fRepeatDelay = 1.0f / 40.0f; // in sec (similar to Windows default but might differ from actual setting)
		const float fHitchDelay  = 1.0f / 10.0f; // in sec. Very low, but still reasonable frame-rate (debug builds)

		m_fRepeatTimer -= Env::Timer()->GetRealFrameTime();              // works even when time is manipulated
		m_fRepeatTimer -= Env::Timer()->GetFrameTime(ITimer::ETIMER_UI); // can be used once ETIMER_UI works even with t_FixedTime

		if (m_fRepeatTimer <= 0.0f)
		{
			if (m_fRepeatTimer < -fHitchDelay)
			{
				// bad framerate or hitch
				m_nRepeatEvent.keyId = eKI_Unknown;
			}
			else
			{
				ProcessInput(m_nRepeatEvent);
				m_fRepeatTimer = fRepeatDelay; // next repeat even in .. sec
			}
		}
	}

#if 0
	CNotificationNetworkConsole::Update();
#endif
}

void CXConsole::Draw()
{
	if (!m_nTempScrollMax)
		return;

	if (!m_pRenderer)
		m_pRenderer = m_system.GetIRenderer(); // For Editor.

	if (!m_pRenderer)
		return;

	if (!m_pFont)
	{
#if 0
		// For Editor.
		ICryFont* pICryFont = m_system.GetICryFont();

		if (pICryFont)
			m_pFont = m_system.GetICryFont()->GetFont("default");
#endif
	}

	ScrollConsole();

	if (!m_bConsoleActive && con_display_last_messages == 0)
		return;

	if (m_nScrollPos <= 0)
	{
		DrawBuffer(70, "console");
	}
	else
	{
		// cursor blinking
		{
			m_fCursorBlinkTimer += Env::Timer()->GetRealFrameTime(); // works even when time is manipulated
			//m_fCursorBlinkTimer += Env::Timer()->GetFrameTime(ITimer::ETIMER_UI); // can be used once ETIMER_UI works even with t_FixedTime

			const float fCursorBlinkDelay = 0.5f; // in sec (similar to Windows default but might differ from actual setting)

			if (m_fCursorBlinkTimer > fCursorBlinkDelay)
			{
				m_bDrawCursor       = !m_bDrawCursor;
				m_fCursorBlinkTimer = 0.0f;
			}
		}

		// TODO: relative/normalized coordinate system in screen-space
		if (!m_nProgressRange)
		{
			if (m_bStaticBackground)
			{
				//IRenderAuxImage::DrawImage(0.0f, 0.0f, float(m_pRenderer->GetWidth()), float(m_pRenderer->GetHeight()), m_pImage ? m_pImage->getId() : m_nWhiteTexID, 0.0f, 0.0f, 1.0f, 1.0f, 0,0,0,0.99);
				//IRenderAuxImage::DrawImage(0.0f, 0.0f, float(m_pRenderer->GetWidth()), float(m_pRenderer->GetHeight()), 12, 0.0f, 0.0f, 1.0f, 1.0f, 0,0,0,0.99);
				Env::Renderer()->Draw2dImage(0.0f, 0.0f, float(m_pRenderer->GetWidth()), float(m_nScrollPos), m_pImage ? m_pImage->GetTextureID() : m_nWhiteTexID, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0, 0, 0, 0.99);
			}
			else
			{
				float fReferenceSize = 600.0f;
				float fSizeX         = (float)m_pRenderer->GetWidth();
				float s0, s1;
				s0 = s1      = (float)fmod(Env::Timer()->GetCurrTime(), 800) / 800.f * 10;
				float fSizeY = (float)m_nTempScrollMax; // * m_pRenderer->GetHeight() / fReferenceSize;

#if 0
				IRenderAuxImage::DrawImage(0, 0, fSizeX, fSizeY, m_nWhiteTexID, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.7f);
				IRenderAuxImage::DrawImage(0, fSizeY, fSizeX, 2.0f * m_pRenderer->GetHeight() / fReferenceSize, m_nWhiteTexID, 0, 0, 0, 0, 0.0f, 0.0f, 0.0f, 1.0f);
#else
				Env::Renderer()->Draw2dImage(0, 0, fSizeX, fSizeY, m_nWhiteTexID, s0, 0.0f, s0 + 1, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.98f);
				Env::Renderer()->Draw2dImage(0, 0, fSizeX, fSizeY, m_nWhiteTexID, s0, 0.0f, s0 - 1, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.98f);
				//Env::Renderer()->Draw2dImage(0, fSizeY, fSizeX, 2.0f * m_pRenderer->GetHeight() / fReferenceSize, m_nWhiteTexID, 0, 0, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
#endif
			}
		}

		// draw progress bar
		if (m_nProgressRange)
			m_pRenderer->DrawImage(0.0f, 0.0f, float(m_pRenderer->GetWidth()), float(m_pRenderer->GetHeight()), m_nLoadingBackTexID, 0.0f, 1.0f, 1.0f, 0.0f, 0, 0, 0, 1);

		DrawBuffer(m_nScrollPos, "console");
	}
}

void CXConsole::AddCommand(const char* sCommand, const char* sScriptFunc, DWORD nFlags, const char* sHelp)
{
	AssertName(sCommand);

	if (m_mapCommands.find(sCommand) == m_mapCommands.end())
	{
		CConsoleCommand cmd;
		cmd.m_sName    = sCommand;
		cmd.m_sCommand = sScriptFunc;
		if (sHelp)
			cmd.m_sHelp = sHelp;
		cmd.m_nFlags = nFlags;
		m_mapCommands.insert(std::make_pair(cmd.m_sName, cmd));
	}
	else
	{
		Env::Log()->LogError("[CVARS]: [DUPLICATE] CXConsole::AddCommand(): script command [%s] is already registered", sCommand);
#if LOG_CVAR_INFRACTIONS_CALLSTACK
		Env::System()->debug_LogCallStack();
#endif
	}
}

void CXConsole::AddCommand(const char* szCommand, ConsoleCommandFunc func, int nFlags, const char* sHelp, bool bIsManagedExternally)
{
	AssertName(szCommand);

	if (m_configVars.find(szCommand) != m_configVars.end())
	{
		Env::Log()->LogWarning("The command '%s' appeared in a config file, but only CVars are allowed in config files.", szCommand);
	}

	if (m_mapCommands.find(szCommand) == m_mapCommands.end())
	{
		CConsoleCommand cmd;
		cmd.m_sName               = szCommand;
		cmd.m_func                = func;
		cmd.m_isManagedExternally = bIsManagedExternally;
		if (sHelp)
			cmd.m_sHelp = sHelp;
		cmd.m_nFlags      = nFlags;
		auto commandIt    = m_mapCommands.insert(std::make_pair(cmd.m_sName, cmd)).first;

		// See if this command was already executed by a config
		// If so we need to execute it immediately
		auto commandRange = m_configCommands.equal_range(szCommand);
		for (auto commandPair = commandRange.first; commandPair != commandRange.second; ++commandPair)
		{
			//string arguments = string().Format("%s %s", szCommand, commandPair->second.c_str());
			char arguments[512];
			sprintf(arguments, "%s %s", szCommand, commandPair->second.c_str());
			auto args = string(arguments);
			ExecuteCommand(commandIt->second, args);
		}

		// Remove all entries
		m_configCommands.erase(commandRange.first, commandRange.second);
	}
	else
	{
		Env::Log()->LogError("[CVARS]: [DUPLICATE] CXConsole::AddCommand(): console command [%s] is already registered", szCommand);
#if LOG_CVAR_INFRACTIONS_CALLSTACK
		Env::System()->debug_LogCallStack();
#endif
	}
}

void CXConsole::ExecuteString(const char* command, bool bNeedSlash /* = false*/, bool bIgnoreDevMode /* = false*/)
{
	bool bDeferExecution = false;
	bool bSilentMode     = false;
	if (!m_deferredExecution && !bDeferExecution)
	{
		// This is a regular mode
		ExecuteStringInternal(command, false, bSilentMode); // not from console
		return;
	}

	// Store the string commands into a list and defer the execution for later.
	// The commands will be processed in CXConsole::Update()
	string str(command);
	str               = TrimLeft(str);

	const bool unroll = false; //	(0 == str.Left(strlen("exec")).compareNoCase("exec"));
	if (unroll)
	{
		const bool oldDeferredExecution = m_deferredExecution;

		// Make sure that the unrolled commands are processed with deferred mode on
		m_deferredExecution             = true;
		ExecuteStringInternal(str.c_str(), false, bSilentMode);
		m_deferredExecution = oldDeferredExecution;
	}
	else
	{
		m_deferredCommands.push_back(SDeferredCommand(str.c_str(), bSilentMode));
	}
}

void CXConsole::Exit(const char* szExitComments, ...)
{
	char sResultMessageText[1024];

	if (szExitComments)
	{
		va_list arglist;
		va_start(arglist, szExitComments);
		vsprintf(sResultMessageText, szExitComments, arglist);
		va_end(arglist);
	}
	else
	{
		strcpy(sResultMessageText, "No comments from application");
	}

	CryFatalError("%s", sResultMessageText);
}

bool CXConsole::IsOpened()
{
	return m_nScrollPos == m_nTempScrollMax;
}

int CXConsole::GetNumVars()
{
	bool bIncludeCommands = true;
	return m_mapVariables.size() + (bIncludeCommands ? m_mapCommands.size() : 0);
}

void CXConsole::GetSortedVars(const char** pszArray, size_t numItems)
{
	GetSortedVars(pszArray, numItems, 0, 0);
}

size_t CXConsole::GetSortedVars(const char** pszArray, size_t numItems, const char* szPrefix, int nListTypes) const
{
	CRY_ASSERT(pszArray != nullptr);
	if (pszArray == nullptr)
		return 0;

	size_t       itemAdded  = 0;
	const size_t iPrefixLen = szPrefix ? strlen(szPrefix) : 0;

	// variables
	if (nListTypes == 0 || nListTypes == 1)
	{
		for (auto& it : m_mapVariables)
		{
			if (itemAdded >= numItems)
				break;

			if (szPrefix && strnicmp(it.first.data(), szPrefix, iPrefixLen) != 0)
				continue;

			if (it.second->GetFlags() & VF_INVISIBLE)
				continue;

			pszArray[itemAdded] = it.first.data();
			itemAdded++;
		}
	}

	// commands
	if (nListTypes == 0 || nListTypes == 2)
	{
		for (auto& it : m_mapCommands)
		{
			if (itemAdded >= numItems)
				break;

			if (szPrefix && strnicmp(it.first.c_str(), szPrefix, iPrefixLen) != 0)
				continue;

			if (it.second.m_nFlags & VF_INVISIBLE)
				continue;

			pszArray[itemAdded] = it.first.c_str();
			itemAdded++;
		}
	}

	if (itemAdded != 0)
		std::sort(pszArray, pszArray + itemAdded, less_CVar);

	return itemAdded;
}

const char* CXConsole::AutoComplete(const char* substr)
{
	// following code can be optimized
	std::vector<const char*> cmds;
	cmds.resize(GetNumVars() + m_mapCommands.size());
	const size_t cmdCount  = GetSortedVars(&cmds[0], cmds.size(), 0, 0);
	const size_t substrLen = strlen(substr);

	// If substring is empty return first command.
	if (substrLen == 0 && cmdCount > 0)
		return cmds[0];

	// find next
	for (size_t i = 0; i < cmdCount; i++)
	{
		const char*  szCmd  = cmds[i];
		const size_t cmdlen = strlen(szCmd);
		if (cmdlen >= substrLen && memcmp(szCmd, substr, substrLen) == 0)
		{
			if (substrLen == cmdlen)
			{
				i++;
				if (i < cmdCount)
					return cmds[i];
				return cmds[i - 1];
			}
			return cmds[i];
		}
	}

	// then first matching case insensitive
	for (size_t i = 0; i < cmdCount; i++)
	{
		const char*  szCmd  = cmds[i];
		const size_t cmdlen = strlen(szCmd);
		if (cmdlen >= substrLen && memicmp(szCmd, substr, substrLen) == 0)
		{
			if (substrLen == cmdlen)
			{
				i++;
				if (i < cmdCount)
					return cmds[i];
				return cmds[i - 1];
			}
			return cmds[i];
		}
	}

	// Not found.
	return "";
}

const char* CXConsole::AutoCompletePrev(const char* substr)
{
	std::vector<const char*> cmds;
	cmds.resize(GetNumVars() + m_mapCommands.size());
	const size_t cmdCount = GetSortedVars(&cmds[0], cmds.size(), 0, 0);

	// If substring is empty return last command.
	if (strlen(substr) == 0 && cmds.size() > 0)
		return cmds[cmdCount - 1];

	for (unsigned int i = 0; i < cmdCount; i++)
	{
		if (stricmp(substr, cmds[i]) == 0)
		{
			if (i > 0)
				return cmds[i - 1];
			else
				return cmds[0];
		}
	}
	return AutoComplete(substr);
}

const char* CXConsole::ProcessCompletion(const char* szInputBuffer)
{
	m_sInputBuffer                = szInputBuffer;
	const bool isBackgroundServer = szInputBuffer[0] == BACKGROUND_SERVER_CHAR;
	if (isBackgroundServer)
		m_sInputBuffer = szInputBuffer + 1;

	const int offset = (szInputBuffer[0] == '\\' ? 1 : 0); // legacy support
	if ((m_sPrevTab.size() > strlen(szInputBuffer + offset)) || strnicmp(m_sPrevTab.c_str(), (szInputBuffer + offset), m_sPrevTab.size()))
	{
		m_nTabCount = 0;
		m_sPrevTab  = "";
	}

	if (m_sInputBuffer.empty())
		return const_cast<char*>(m_sInputBuffer.c_str());

	const bool showlist = !m_nTabCount && m_sPrevTab == "";

	if (m_nTabCount == 0)
	{
		if (m_sInputBuffer.size() > 0)
		{
			if (m_sInputBuffer[0] == '\\')
				m_sPrevTab = &m_sInputBuffer.c_str()[1]; // legacy support
			else
				m_sPrevTab = m_sInputBuffer;
		}
		else
		{
			m_sPrevTab = "";
		}
	}

	// try to search in command list
	bool                bArgumentAutoComplete = false;
	std::vector<string> matches;
	if (m_sPrevTab.find(' ') != string::npos)
	{
		bool         bProcessAutoCompl = true;

		// Find command.
		const string sVar              = m_sPrevTab.substr(0, m_sPrevTab.find(' '));
		const ICVar* pCVar             = GetCVar(sVar.data());
		if (pCVar)
		{
			if (!(pCVar->GetFlags() & VF_RESTRICTEDMODE) && con_restricted) // in restricted mode we allow only VF_RESTRICTEDMODE CVars&CCmd
				bProcessAutoCompl = false;
		}

		auto it = m_mapCommands.find(sVar);
		if (it != m_mapCommands.end())
		{
			const CConsoleCommand& ccmd = it->second;
			if (!(ccmd.m_nFlags & VF_RESTRICTEDMODE) && con_restricted) // in restricted mode we allow only VF_RESTRICTEDMODE CVars&CCmd
				bProcessAutoCompl = false;
		}

		if (bProcessAutoCompl)
		{
			const IConsoleArgumentAutoComplete* pArgumentAutoComplete = stl::find_in_map(m_mapArgumentAutoComplete, sVar, nullptr);
			if (pArgumentAutoComplete)
			{
				const int nMatches = pArgumentAutoComplete->GetCount();
				for (int i = 0; i < nMatches; i++)
				{
					const string cmd = string(sVar) + " " + pArgumentAutoComplete->GetValue(i);
					if (strnicmp(m_sPrevTab.c_str(), cmd.c_str(), m_sPrevTab.length()) == 0 && Env::System()->IsCVarWhitelisted(cmd.c_str(), true))
					{
						bArgumentAutoComplete = true;
						matches.push_back(cmd);
					}
				}
			}
		}
	}

	if (!bArgumentAutoComplete)
	{
		for (auto& pair : m_mapCommands)
		{
			const CConsoleCommand& cmd = pair.second;
			if ((cmd.m_nFlags & VF_RESTRICTEDMODE) || !con_restricted) // in restricted mode we allow only VF_RESTRICTEDMODE CVars&CCmd
			{
				if (strnicmp(m_sPrevTab.c_str(), pair.first.c_str(), m_sPrevTab.length()) == 0 && Env::System()->IsCVarWhitelisted(pair.first.c_str(), true))
				{
					matches.push_back(pair.first);
				}
			}
		}

		// try to search in console variables
		for (auto& pair : m_mapVariables)
		{
			const ICVar* pVar = pair.second;
#ifdef _RELEASE
			if (!Env::Get()->IsEditor())
			{
				const bool isCheat = (pVar->GetFlags() & (VF_CHEAT | VF_CHEAT_NOCHECK | VF_CHEAT_ALWAYS_CHECK)) != 0;
				if (isCheat)
				{
					continue;
				}
			}
#endif

			if ((pVar->GetFlags() & VF_RESTRICTEDMODE) || !con_restricted) // in restricted mode we allow only VF_RESTRICTEDMODE CVars&CCmd
			{
				if (strnicmp(m_sPrevTab.c_str(), pair.first.data(), m_sPrevTab.length()) == 0 && Env::System()->IsCVarWhitelisted(pair.first.data(), true))
				{
					matches.push_back(pair.first);
				}
			}
		}
	}

	if (!matches.empty())
		std::sort(matches.begin(), matches.end(), less_CVar); // to sort commands with variables

	if (showlist && !matches.empty())
	{
		ConsoleLogInput(" "); // empty line before auto completion
		for (const string& match : matches)
		{
			ICVar* pVar = GetCVar(match.data());
			if (pVar)
				DisplayVarValue(pVar);
			else
				ConsoleLogInputResponse("    $3%s $6(Command)", match.c_str());
		}
	}

	int nMatch = 0;
	for (const string& match : matches)
	{
		if (m_nTabCount <= nMatch)
		{
			m_sInputBuffer = match;
			m_sInputBuffer += " ";
			m_nTabCount = nMatch + 1;
#if 0
			if (isBackgroundServer)
				m_sInputBuffer.insert(0, BACKGROUND_SERVER_CHAR);
#endif

			return (m_sInputBuffer.c_str());
		}
		nMatch++;
	}

	if (m_nTabCount > 0)
	{
		m_nTabCount    = 0;
		m_sInputBuffer = m_sPrevTab;
		m_sInputBuffer = ProcessCompletion(m_sInputBuffer.c_str());
	}

#if 0
	if (isBackgroundServer)
		m_sInputBuffer.insert(0, BACKGROUND_SERVER_CHAR);
#endif

	return m_sInputBuffer.c_str();
}

void CXConsole::RegisterAutoComplete(const char* sVarOrCommand, IConsoleArgumentAutoComplete* pArgAutoComplete)
{
	m_mapArgumentAutoComplete[sVarOrCommand] = pArgAutoComplete;
}

void CXConsole::UnRegisterAutoComplete(const char* sVarOrCommand)
{
	auto it = m_mapArgumentAutoComplete.find(sVarOrCommand);
	if (it != m_mapArgumentAutoComplete.end())
		m_mapArgumentAutoComplete.erase(it);
}

void CXConsole::ResetAutoCompletion()
{
	m_nTabCount = 0;
	m_sPrevTab  = "";
}

void CXConsole::DumpCommandsVars(char* prefix)
{
	FILE* f0 = fopen("consolecommandsandvars.txt", "w");

	if (!f0)
		return;

	fprintf(f0, " CHEAT: stays in the default value if cheats are not disabled\n");
	fprintf(f0, " REQUIRE_NET_SYNC: cannot be changed on client and when connecting it's sent to the client\n");
	fprintf(f0, " SAVEGAME: stored when saving a savegame\n");
	fprintf(f0, " READONLY: can not be changed by the user\n");
	fprintf(f0, "-------------------------\n");
	fprintf(f0, "\n");

	for (auto& pair : m_mapCommands)
	{
		const CConsoleCommand& cmd = pair.second;
		if (hasprefix(cmd.m_sName.c_str(), prefix))
		{
			const char* sFlags = GetFlagsString(cmd.m_nFlags);
			fprintf(f0, "Command: %s %s\nscript: %s\nhelp: %s\n\n", cmd.m_sName.c_str(), sFlags, cmd.m_sCommand.c_str(), cmd.m_sHelp.c_str());
		}
	}

	for (auto& pair : m_mapVariables)
	{
		const ICVar* var = pair.second;
		if (hasprefix(var->GetName(), prefix))
		{
			const char* sFlags = GetFlagsString(var->GetFlags());
			fprintf(f0, "variable: %s %s\ntype: %s\ncurrent: %s\nhelp: %s\n\n", var->GetName(), sFlags, ECVarTypeHelper::GetNameForECVar(ECVarType(var->GetType())), var->GetString(), var->GetHelp());
		}
	}

	fclose(f0);

	ConsoleLogInputResponse("successfully wrote consolecommandsandvars.txt");
}

void CXConsole::GetMemoryUsage(ICrySizer* pSizer) const
{
	pSizer->AddObject(this, sizeof(*this));
	pSizer->AddObject(m_sInputBuffer);
	pSizer->AddObject(m_sPrevTab);
	pSizer->AddObject(m_dqConsoleBuffer);
	pSizer->AddObject(m_dqHistory);
	pSizer->AddObject(m_mapCommands);
	pSizer->AddObject(m_mapBinds);
}

void CXConsole::ResetProgressBar(int nProgressBarRange)
{
#if 1
	return;
#endif
	m_nProgressRange = nProgressBarRange;
	m_nProgress      = 0;

	if (nProgressBarRange < 0)
		nProgressBarRange = 0;

	if (!m_nProgressRange)
	{
		if (m_nLoadingBackTexID)
		{
#if 0
			if (m_pRenderer)
				m_pRenderer->RemoveTexture(m_nLoadingBackTexID);
#endif
			m_nLoadingBackTexID = -1;
		}
	}

	static ICVar* log_Verbosity = GetCVar("log_Verbosity");

	if (log_Verbosity && (!log_Verbosity->GetIVal()))
		Clear();
}

void CXConsole::TickProgressBar()
{
#if 1
	return;
#endif
	if (m_nProgressRange != 0 && m_nProgressRange > m_nProgress)
	{
		m_nProgress++;
		m_system.UpdateLoadingScreen();
	}
#if 0
	if (m_system.GetIRenderer())
		m_system.GetIRenderer()->FlushRTCommands(false, false, false); // Try to switch render thread contexts to make RT always busy during loading
#endif
}

void CXConsole::DumpKeyBinds(IKeyBindDumpSink* pCallback)
{
	for (auto& pair : m_mapBinds)
		pCallback->OnKeyBindFound(pair.first.c_str(), pair.second.c_str());
}

const char* CXConsole::FindKeyBind(const char* sCmd)
{
	auto it = m_mapBinds.find(CONST_TEMP_STRING(sCmd));
	if (it != m_mapBinds.end())
		return it->second.c_str();

	return nullptr;
}

void CXConsole::AddConsoleVarSink(IConsoleVarSink* pSink)
{
	m_consoleVarSinks.push_back(pSink);
}

void CXConsole::RemoveConsoleVarSink(IConsoleVarSink* pSink)
{
	m_consoleVarSinks.remove(pSink);
}

const char* CXConsole::GetHistoryElement(const bool bUpOrDown)
{
	if (bUpOrDown)
	{
		if (!m_dqHistory.empty())
		{
			if (m_nHistoryPos < (int)(m_dqHistory.size() - 1))
			{
				m_nHistoryPos++;
				m_sReturnString = m_dqHistory[m_nHistoryPos];
				return m_sReturnString.c_str();
			}
		}
	}
	else
	{
		if (m_nHistoryPos > 0)
		{
			m_nHistoryPos--;
			m_sReturnString = m_dqHistory[m_nHistoryPos];
			return m_sReturnString.c_str();
		}
	}

	return 0;
}

void CXConsole::AddCommandToHistory(const char* szCommand)
{
	CRY_ASSERT(szCommand);
	m_nHistoryPos = -1;

	if (!m_dqHistory.empty())
	{
		// add only if the command is != than the last
		if (m_dqHistory.front() != szCommand)
			m_dqHistory.push_front(szCommand);
	}
	else
		m_dqHistory.push_front(szCommand);

	while (m_dqHistory.size() > MAX_HISTORY_ENTRIES)
		m_dqHistory.pop_back();
}

//enable this for now, we need it for profiling etc
//MUST DISABLE FOR TCG BUILDS
#define PROCESS_XCONSOLE_INPUT

bool CXConsole::OnInputEvent(const SInputEvent& event)
{
#ifdef PROCESS_XCONSOLE_INPUT
	if (event.keyId == eKI_SYS_Commit)
		return false;

	// Process input event
	if (event.state == eIS_Released && m_bConsoleActive)
		m_nRepeatEvent.keyId = eKI_Unknown;

	if (event.state != eIS_Pressed)
		return false;

	// restart cursor blinking
	m_fCursorBlinkTimer           = 0.0f;
	m_bDrawCursor                 = true;

	// key repeat
	const float fStartRepeatDelay = 0.5f; // in sec (similar to Windows default but might differ from actual setting)
	m_nRepeatEvent                = event;
	m_fRepeatTimer                = fStartRepeatDelay;

	//execute Binds
	if (!m_bConsoleActive)
	{
		const char* cmd = 0;

		if (event.modifiers == 0)
		{
			// fast
			cmd = FindKeyBind(event.keyName.c_str());
		}
		else
		{
			// slower
			char szCombinedName[40];
			int  iLen = 0;

			if (event.modifiers & eMM_Ctrl)
			{
				strcpy(szCombinedName, "ctrl_");
				iLen += 5;
			}
			if (event.modifiers & eMM_Shift)
			{
				strcpy(&szCombinedName[iLen], "shift_");
				iLen += 6;
			}
			if (event.modifiers & eMM_Alt)
			{
				strcpy(&szCombinedName[iLen], "alt_");
				iLen += 4;
			}
			if (event.modifiers & eMM_Win)
			{
				strcpy(&szCombinedName[iLen], "win_");
				iLen += 4;
			}

			strcpy(&szCombinedName[iLen], event.keyName.c_str());

			cmd = FindKeyBind(szCombinedName);
		}

		if (cmd)
		{
			SetInputLine("");
			ExecuteStringInternal(cmd, true); // keybinds are treated as they would come from console
		}
	}
	else
	{
		if (event.keyId != eKI_Tab)
			ResetAutoCompletion();

		if (event.keyId == eKI_V && (event.modifiers & eMM_Ctrl) != 0)
		{
			Paste();
			return false;
		}

		if (event.keyId == eKI_C && (event.modifiers & eMM_Ctrl) != 0)
		{
			Copy();
			return false;
		}
	}

	// keep only bare tilde key, modified one may be used by someone else - such as editor suspend
	if (event.keyId == eKI_Tilde && !(event.modifiers & (eMM_Shift | eMM_Ctrl | eMM_Alt)))
	{
		if (m_bActivationKeyEnable)
		{
			m_sInputBuffer = "";
			m_nCursorPos   = 0;
			m_pInput->ClearKeyState();
			ShowConsole(!GetStatus());
			m_nRepeatEvent.keyId = eKI_Unknown;
			return true;
		}
	}
	// Normally, this will notify the editor to switch out of game mode, but in order to allow access to game functionality bound to the Escape key, we skip it if Shift is held down
	if (event.keyId == eKI_Escape && ((event.modifiers & eMM_Shift) == 0 || !Env::Get()->IsEditor()))
	{
		//switch process or page or other things
		m_sInputBuffer = "";
		m_nCursorPos   = 0;

		ShowConsole(false);

		ISystemUserCallback* pCallback = m_system.GetUserCallback();
		if (pCallback)
			pCallback->OnProcessSwitch();

		return false;
	}

	return ProcessInput(event);

#else

	return false;

#endif
}

bool CXConsole::OnInputEventUI(const SUnicodeEvent& event)
{
#ifdef PROCESS_XCONSOLE_INPUT
	if (m_bConsoleActive && !m_readOnly && event.inputChar >= 32 && event.inputChar != 96) // 32: Space // 96: Console toggle
		AddInputChar(event.inputChar);
#endif
	return true;
}
void CXConsole::FindVar(const char* substr)
{
	std::vector<const char*> cmds;
	cmds.resize(GetNumVars() + m_mapCommands.size());
	const size_t cmdCount = GetSortedVars(&cmds[0], cmds.size(), 0, 0);

	for (size_t i = 0; i < cmdCount; i++)
	{
		if (CryStringUtils::stristr(cmds[i], substr))
		{
			if (Env::System()->IsCVarWhitelisted(cmds[i], true))
			{
				ICVar* pCvar = Env::Console()->GetCVar(cmds[i]);
				if (pCvar)
				{
#ifdef _RELEASE
					if (!Env::Get()->IsEditor())
					{
						const bool isCheat = (pCvar->GetFlags() & (VF_CHEAT | VF_CHEAT_NOCHECK | VF_CHEAT_ALWAYS_CHECK)) != 0;
						if (isCheat)
							continue;
					}
#endif
					DisplayVarValue(pCvar);
				}
				else
				{
					ConsoleLogInputResponse("    $3%s $6(Command)", cmds[i]);
				}
			}
		}
	}
}

bool CXConsole::ProcessInput(const SInputEvent& event)
{
#ifdef PROCESS_XCONSOLE_INPUT

	if (!m_bConsoleActive || m_readOnly)
		return false;

	// this is not so super-nice as the XKEY's ... but a small price to pay
	// if speed is a problem (which would be laughable for this) the CCryName
	// can be cached in a static var
	if (event.keyId == eKI_Enter || event.keyId == eKI_NP_Enter)
	{
		ExecuteInputBuffer();
		m_nScrollLine = 0;
		return true;
	}
	else if (event.keyId == eKI_Backspace)
	{
		RemoveInputChar(true);
		return true;
	}
	else if (event.keyId == eKI_Left)
	{
		if (m_nCursorPos)
		{
			const char* pCursor = m_sInputBuffer.c_str() + m_nCursorPos;
	#if 0
			Unicode::CIterator<const char*, false> pUnicode(pCursor);
			--pUnicode; // Note: This moves back one UCS code-point, but doesn't necessarily match one displayed character (ie, combining diacritics)
			pCursor = pUnicode.GetPosition();
			m_nCursorPos = pCursor - m_sInputBuffer.c_str();
	#else
			m_nCursorPos = pCursor - m_sInputBuffer.c_str() - 1;
	#endif
		}
		return true;
	}
	else if (event.keyId == eKI_Right)
	{
		if (m_nCursorPos < (int)(m_sInputBuffer.length()))
		{
			const char* pCursor = m_sInputBuffer.c_str() + m_nCursorPos;
	#if 0
			Unicode::CIterator<const char*, false> pUnicode(pCursor);
			++pUnicode; // Note: This moves forward one UCS code-point, but doesn't necessarily match one displayed character (ie, combining diacritics)
			pCursor = pUnicode.GetPosition();
			m_nCursorPos = pCursor - m_sInputBuffer.c_str();
	#else
			m_nCursorPos = pCursor - m_sInputBuffer.c_str() + 1;
	#endif
		}
		return true;
	}
	else if (event.keyId == eKI_Up)
	{
		const char* szHistoryLine = GetHistoryElement(true); // true=UP

		if (szHistoryLine)
		{
			m_sInputBuffer = szHistoryLine;
			m_nCursorPos   = (int)m_sInputBuffer.size();
		}
		return true;
	}
	else if (event.keyId == eKI_Down)
	{
		const char* szHistoryLine = GetHistoryElement(false); // false=DOWN

		if (szHistoryLine)
		{
			m_sInputBuffer = szHistoryLine;
			m_nCursorPos   = (int)m_sInputBuffer.size();
		}
		return true;
	}
	else if (event.keyId == eKI_Tab)
	{
		if (!(event.modifiers & eMM_Alt))
		{
			m_sInputBuffer = ProcessCompletion(m_sInputBuffer.c_str());
			m_nCursorPos   = m_sInputBuffer.size();
		}
		return true;
	}
	else if (event.keyId == eKI_PgUp || event.keyId == eKI_MouseWheelUp)
	{
		if (event.modifiers & eMM_Ctrl)
			m_nScrollLine = std::min((int)(m_dqConsoleBuffer.size() - 1), m_nScrollLine + 21);
		else
			m_nScrollLine = std::min((int)(m_dqConsoleBuffer.size() - 1), m_nScrollLine + 1);

		return true;
	}
	else if (event.keyId == eKI_PgDn || event.keyId == eKI_MouseWheelDown)
	{
		if (event.modifiers & eMM_Ctrl)
			m_nScrollLine = std::max(0, m_nScrollLine - 21);
		else
			m_nScrollLine = std::max(0, m_nScrollLine - 1);

		return true;
	}
	else if (event.keyId == eKI_Home)
	{
		if (event.modifiers & eMM_Ctrl)
			m_nScrollLine = m_dqConsoleBuffer.size() - 1;
		else
			m_nCursorPos = 0;

		return true;
	}
	else if (event.keyId == eKI_End)
	{
		if (event.modifiers & eMM_Ctrl)
			m_nScrollLine = 0;
		else
			m_nCursorPos = (int)m_sInputBuffer.length();

		return true;
	}
	else if (event.keyId == eKI_Delete)
	{
		RemoveInputChar(false);
		return true;
	}
	else
	{
		// Below is a hack due to pc having character input event being caught when in the editor and also due to inconsistencies in keyboard devices (Some fire OnInputEvent and OnInputEventUI and some only fire OnInputEvent)
		// i.e. OnInputEventUI will never be fired
		// The below isn't true unicode, it's converted from ascii
		// TODO: Rework windows processing of input (WM_CHAR) into CKeyboard (Both cases when in editor and not) and make all keyboard devices consistent and can remove the below code
		if (Env::Get()->IsEditor())
		{
			const uint32 inputChar = m_pInput->GetInputCharUnicode(event);

			if (inputChar)
			{
				AddInputChar(inputChar);
				return true;
			}
		}
	}

#endif

	return false;
}

void CXConsole::DrawBuffer(int nScrollPos, const char* szEffect)
{
	if (m_pFont && m_pRenderer)
	{
		//const int flags = eDrawText_Monospace | eDrawText_CenterV | eDrawText_2D;
		const float fontSize   = con_font_size;
		const float fontScale  = 0.5;
		//const float csize = 1.16f * fontSize;
		const float csize      = fontSize * fontScale;
		const float fCharWidth = 0.5f * csize;

		float       yPos       = nScrollPos - 2 * csize - 3.0f;
		const float xPos       = LINE_BORDER;

		//Draw the input line
		if (m_bConsoleActive && !m_nProgressRange)
		{
			m_pFont->RenderText(
			    ">",
			    //xPos - fCharWidth, yPos, fontScale * 1.16f / 14, &glm::vec4(1)[0]);
			    xPos - fCharWidth, yPos, fontScale, &glm::vec4(1)[0]);
			m_pFont->RenderText(
			    m_sInputBuffer.c_str(),
			    //xPos, yPos, fontScale * 1.16f / 14, &glm::vec4(1)[0]);
			    xPos, yPos, fontScale, &glm::vec4(1)[0]);
#if 0
			IRenderAuxText::DrawText(Legacy::Vec3(xPos - fCharWidth, yPos, 1), fontScale * 1.16f / 14, nullptr, flags, ">");
			IRenderAuxText::DrawText(Legacy::Vec3(xPos, yPos, 1), fontScale * 1.16f / 14, nullptr, flags, m_sInputBuffer.c_str());
#endif

			if (m_bDrawCursor)
			{
				string szCursorLeft(m_sInputBuffer.c_str(), m_sInputBuffer.c_str() + m_nCursorPos);
				//int n = m_pFont->GetTextLength(szCursorLeft.c_str(), false);
				//float n = 1.16f * m_pFont->TextWidth(szCursorLeft);
				float  n = fontScale * m_pFont->TextWidth(szCursorLeft);

				//IRenderAuxText::DrawText(Legacy::Vec3(xPos + (fCharWidth * n), yPos, 1), fontScale * 1.16f / 14, nullptr, flags, "_");
				m_pFont->RenderText(
				    "_",
				    //xPos + (/*fCharWidth **/ n), yPos, fontScale * 1.16f / 14, &glm::vec4(1)[0]);
				    xPos + (/*fCharWidth **/ n), yPos, fontScale, &glm::vec4(1)[0]);
			}
		}

		yPos -= csize;

		int  nScroll = 0;
		auto ritor   = m_dqConsoleBuffer.rbegin();
		while (ritor != m_dqConsoleBuffer.rend() && yPos >= 0)
		{
			if (nScroll >= m_nScrollLine)
			{
				const char* buf = ritor->c_str(); // GetBuf(k);

				//FIXME: exception on disconnect console command
				if (*buf > 0 && *buf < 32)
					buf++; // to jump over verbosity level character

				if (yPos + csize > 0)
				{
#if 0
					IRenderAuxText::DrawText(Legacy::Vec3(xPos, yPos, 1), fontScale * 1.16f / 14, nullptr, flags, buf);
#else
					m_pFont->RenderText(
					    buf,
					    //xPos - fCharWidth, yPos, fontScale * 1.16f / 14, &glm::vec4(1)[0]);
					    xPos - fCharWidth, yPos, fontScale, &glm::vec4(1)[0]);
#endif
				}
				yPos -= csize;
			}
			++nScroll;
			++ritor;
		}
		//m_pFont->Submit();
	}
}

void CXConsole::AddLine(const char* inputStr)
{
	string str = inputStr;

	// strip trailing \n or \r.
	if (!str.empty() && (str[str.size() - 1] == '\n' || str[str.size() - 1] == '\r'))
		str.resize(str.size() - 1);

	string::size_type nPos;
	while ((nPos = str.find('\n')) != string::npos)
	{
		str.replace(nPos, 1, 1, ' ');
	}

	while ((nPos = str.find('\r')) != string::npos)
	{
		str.replace(nPos, 1, 1, ' ');
	}

	m_dqConsoleBuffer.push_back(str);
	const int nBufferSize = con_line_buffer_size;

	while (((int)(m_dqConsoleBuffer.size())) > nBufferSize)
	{
		m_dqConsoleBuffer.pop_front();
	}

	// tell everyone who is interested (e.g. dedicated server printout)
	for (auto* sink : m_OutputSinks)
		sink->Print(str.c_str());
}

void CXConsole::AddLinePlus(const char* inputStr)
{
	if (!m_dqConsoleBuffer.size())
		return;

	string str = inputStr;
	if (!str.empty() && (str[str.size() - 1] == '\n' || str[str.size() - 1] == '\r')) // strip trailing \n or \r.
		str.resize(str.size() - 1);

	string::size_type nPos;
	while ((nPos = str.find('\n')) != string::npos)
		str.replace(nPos, 1, 1, ' ');

	while ((nPos = str.find('\r')) != string::npos)
		str.replace(nPos, 1, 1, ' ');

	const string tmpStr = m_dqConsoleBuffer.back();
	m_dqConsoleBuffer.pop_back();

	if (tmpStr.size() < 256)
		m_dqConsoleBuffer.push_back(tmpStr + str);
	else
		m_dqConsoleBuffer.push_back(tmpStr);

	// tell everyone who is interested (e.g. dedicated server printout)
	for (IOutputPrintSink* pSink : m_OutputSinks)
		pSink->Print((tmpStr + str).c_str());
}

void CXConsole::LoadConfigVar(const char* szVariable, const char* sValue)
{
	ICVar* pCVar = GetCVar(szVariable);
	if (pCVar)
	{
		const bool isConst          = pCVar->IsConstCVar();
		const bool isCheat          = ((pCVar->GetFlags() & (VF_CHEAT | VF_CHEAT_NOCHECK | VF_CHEAT_ALWAYS_CHECK)) != 0);
		const bool isReadOnly       = ((pCVar->GetFlags() & VF_READONLY) != 0);
		const bool isDeprecated     = ((pCVar->GetFlags() & VF_DEPRECATED) != 0);
		const bool wasInConfig      = ((pCVar->GetFlags() & VF_WASINCONFIG) != 0);
		const bool fromSystemConfig = ((pCVar->GetFlags() & VF_SYSSPEC_OVERWRITE) != 0);
		bool       allowChange      = true;

		if ((
#if CVAR_GROUPS_ARE_PRIVILEGED
		        !m_bIsProcessingGroup &&
#endif
		        (isConst || isCheat || isReadOnly)) ||
		    isDeprecated)
		{
			allowChange = (!isDeprecated && Env::System()->IsDevMode()) || Env::Get()->IsEditor();
			if (!(Env::Get()->IsEditor()) || isDeprecated)
			{
#if LOG_CVAR_INFRACTIONS
				LogChangeMessage(pCVar->GetName(), isConst, isCheat,
				                 isReadOnly, isDeprecated, pCVar->GetString(), sValue, m_bIsProcessingGroup, allowChange);
	#if LOG_CVAR_INFRACTIONS_CALLSTACK
				Env::System()->debug_LogCallStack();
	#endif
#endif
			}
		}

		if (m_currentLoadConfigType != eLoadConfigInit && wasInConfig && fromSystemConfig)
		{
			// When trying to change cvar not by loading standard configs (from sys specs), and cvar was present in system.cfg, protect it and not allow sys spec to change it.
			allowChange = false;
		}

		if (allowChange)
		{
#if 0
			pCVar->SetFromString(sValue);
#else
			pCVar->ForceSet(sValue);
#endif

			if (m_currentLoadConfigType == eLoadConfigInit || m_currentLoadConfigType == eLoadConfigDefault)
				pCVar->SetFlags(pCVar->GetFlags() | VF_WASINCONFIG);

			if (m_currentLoadConfigType == eLoadConfigInit)
				pCVar->SetFlags(pCVar->GetFlags() | VF_SYSSPEC_OVERWRITE);
		}
		return;
	}

	if (!CRY_VERIFY(m_mapCommands.find(szVariable) == m_mapCommands.end(), "Tried to load config CVar '%s', but it is registered as a command.", szVariable))
	{
		return;
	}

	auto configVar = m_configVars.find(szVariable);
	if (configVar != m_configVars.end())
	{
		uint32 nCVarFlags = configVar->second.nCVarOrFlags;
		// If this cvar was already previously loaded, check that we are allowed to override its value.
		if (m_currentLoadConfigType != eLoadConfigInit && (nCVarFlags & VF_SYSSPEC_OVERWRITE) && (nCVarFlags & VF_WASINCONFIG))
		{
			// This cvar was already loaded and with higher priority config type.
			// So ignore overriding its value
			return;
		}
	}

	SConfigVar temp;
	temp.m_value       = sValue;
	temp.m_partOfGroup = m_bIsProcessingGroup;
	temp.nCVarOrFlags  = VF_WASINCONFIG;

	if (m_currentLoadConfigType == eLoadConfigInit)
		temp.nCVarOrFlags |= VF_SYSSPEC_OVERWRITE;

	m_configVars[szVariable] = temp;
}

void CXConsole::LoadConfigCommand(const char* szCommand, const char* szArguments)
{
	auto it = m_mapCommands.find(szCommand);
	if (it == m_mapCommands.end())
	{
		if (CRY_VERIFY(GetCVar(szCommand) == nullptr, "Tried to load config command '%s', but it is registered as a CVar.", szCommand))
		{
			m_configCommands.emplace(szCommand, szArguments);
		}
	}
	else
	{
		string arguments = szCommand + string(" ") + szArguments;
		ExecuteCommand(it->second, arguments);
	}
}

void CXConsole::ExecuteStringInternal(const char* command, const bool bFromConsole, const bool bSilentMode)
{
	CRY_ASSERT(command);
	CRY_ASSERT(command[0] != '\\', "ExecuteStringInternal command should not start with \"\\\"");

#if !defined(RELEASE) || defined(ENABLE_DEVELOPER_CONSOLE_IN_RELEASE)
	if (command[0] == BACKGROUND_SERVER_CHAR)
	{
		// Send to background server
		if (!con_restricted || !bFromConsole)
		{
			ExecuteStringInternal((string("cmd_server_command ") + string(command + 1)).data(), bFromConsole, bSilentMode);
			return;
		}
	}

	// Execute as string
	if (command[0] == '#' || command[0] == '@')
	{
		if (!con_restricted || !bFromConsole) // in restricted mode we allow only VF_RESTRICTEDMODE CVars&CCmd
		{
			AddLine(command);

			if (m_system.IsDevMode())
			{
				if (m_system.GetIScriptSystem())
					m_system.GetIScriptSystem()->ExecuteBuffer(command + 1, strlen(command) - 1);
				m_bDrawCursor = false;
			}
			return;
		}
	}
#endif

	std::list<string> lineCommands;
	SplitCommands(command, lineCommands);

	while (!lineCommands.empty())
	{
		string       sTemp        = lineCommands.front();
		string       sCommand     = lineCommands.front();
		const string sLineCommand = sCommand;
		lineCommands.pop_front();

		if (!bSilentMode)
			if (GetStatus())
				AddLine(sTemp.data());

		string::size_type nPos = sTemp.find_first_of('=');

		if (nPos != string::npos)
			sCommand = sTemp.substr(0, nPos);
		else if ((nPos = sTemp.find_first_of(' ')) != string::npos)
			sCommand = sTemp.substr(0, nPos);
		else
			sCommand = sTemp;

#if 0
		sCommand.Trim();
#else
		sCommand = Trim(sCommand);
#endif

		// Search for CVars
		if (sCommand.length() > 1 && sCommand[0] == '?')
		{
			sTemp = sCommand.substr(1);
			FindVar(sTemp.c_str());
			continue;
		}

		// Check if is a command
		auto itrCmd = m_mapCommands.find(sCommand);
		if (itrCmd != m_mapCommands.end())
		{
			if (((itrCmd->second).m_nFlags & VF_RESTRICTEDMODE) || !con_restricted || !bFromConsole) // in restricted mode we allow only VF_RESTRICTEDMODE CVars&CCmd
			{
				if (itrCmd->second.m_nFlags & VF_BLOCKFRAME)
					m_blockCounter++;

				sTemp = sLineCommand;
				ExecuteCommand((itrCmd->second), sTemp);

				continue;
			}
		}

		// Check  if is a variable
		auto itrVar = m_mapVariables.find(sCommand);
		if (itrVar != m_mapVariables.end())
		{
			ICVar* pCVar = itrVar->second;

			if ((pCVar->GetFlags() & VF_RESTRICTEDMODE) || !con_restricted || !bFromConsole) // in restricted mode we allow only VF_RESTRICTEDMODE CVars&CCmd
			{
				if (pCVar->GetFlags() & VF_BLOCKFRAME)
					m_blockCounter++;

				if (nPos != string::npos)
				{
					sTemp = sTemp.substr(nPos + 1); // remove the command from sTemp
#if 0
					sTemp.Trim(" \t\r\n\"\'");
#else
					sTemp = Trim(sTemp, " \t\r\n\"\'");
#endif

					if (sTemp == "?")
					{
						ICVar* v = itrVar->second;
						DisplayHelp(v->GetHelp(), sCommand.c_str());
						return;
					}

					if (!sTemp.empty() || (ECVarType(pCVar->GetType()) == ECVarType::String))
					{
						// renderer cvars will be updated in the render thread
						if ((pCVar->GetFlags() & VF_RENDERER_CVAR) && m_pRenderer)
						{
#if 0
							m_pRenderer->SetRendererCVar(pCVar, sTemp.c_str(), bSilentMode);
#endif
							continue;
						}

						pCVar->SetFromString(sTemp.c_str());
					}
				}

				// the following line calls AddLine() indirectly
				if (!bSilentMode)
					DisplayVarValue(pCVar);

				continue;
			}
		}

		if (!bSilentMode)
			ConsoleWarning("Unknown command: %s", sCommand.c_str());
	}
}

void CXConsole::ExecuteDeferredCommands()
{
	if (m_waitFrames)
	{
		--m_waitFrames;
		return;
	}

#if 0
	if (m_waitSeconds.GetValue())
	{
		if (m_waitSeconds > Env::Timer()->GetFrameStartTime())
			return;

		m_waitSeconds.SetValue(0); 	// Help to avoid overflow problems
	}
#endif

	const int blockCounter = m_blockCounter;

#if 0
	while (m_waitFrames == 0 && m_waitSeconds.GetValue() == 0 && !m_deferredCommands.empty())
	{
		auto& deferredCommand = m_deferredCommands.front();
		ExecuteStringInternal(deferredCommand.command.c_str(), false, deferredCommand.silentMode);
		m_deferredCommands.pop_front();

		// A blocker command was executed
		if (m_blockCounter != blockCounter)
			break;
	}
#endif
}

const char* CXConsole::GetFlagsString(const uint32 dwFlags)
{
	static char sFlags[256];

	strcpy(sFlags, "");

	if (dwFlags & VF_READONLY) strcat(sFlags, "READONLY, ");
	if (dwFlags & VF_DEPRECATED) strcat(sFlags, "DEPRECATED, ");
	if (dwFlags & VF_DUMPTODISK) strcat(sFlags, "DUMPTODISK, ");
	if (dwFlags & VF_REQUIRE_LEVEL_RELOAD) strcat(sFlags, "REQUIRE_LEVEL_RELOAD, ");
	if (dwFlags & VF_REQUIRE_APP_RESTART) strcat(sFlags, "REQUIRE_APP_RESTART, ");
	if (dwFlags & VF_RESTRICTEDMODE) strcat(sFlags, "RESTRICTEDMODE, ");

	if (sFlags[0] != 0)
		sFlags[strlen(sFlags) - 2] = 0; // remove ending ", "

	return sFlags;
}

void CXConsole::AddInputChar(const uint32 c)
{
// Convert UCS code-point into UTF-8 string
#if 0
	char utf8_buf[5];
	Unicode::Convert(utf8_buf, c);
#else
	char utf8_buf;
	auto res = wcrtomb(&utf8_buf, c, nullptr);
#endif

	if (m_nCursorPos < (int)(m_sInputBuffer.length()))
		m_sInputBuffer.insert(m_nCursorPos, 1, utf8_buf);
	else
		m_sInputBuffer = m_sInputBuffer + utf8_buf;
#if 0
	m_nCursorPos += strlen(utf8_buf);
#else
	m_nCursorPos++;
#endif
}

void CXConsole::RemoveInputChar(bool bBackSpace)
{
	if (m_sInputBuffer.empty())
		return;

	if (bBackSpace)
	{
		if (m_nCursorPos > 0)
		{
			const char* const pBase   = m_sInputBuffer.c_str();
			const char*       pCursor = pBase + m_nCursorPos;
			const char* const pEnd    = pCursor;
#if 0
			Unicode::CIterator<const char*, false> pUnicode(pCursor);
			pUnicode--; // Remove one UCS code-point, doesn't account for combining diacritics
			pCursor = pUnicode.GetPosition();
			const size_t length = pEnd - pCursor;
#endif
			pCursor--;
			const size_t length = pEnd - pCursor;
			m_sInputBuffer.erase(pCursor - pBase, length);
			m_nCursorPos -= length;
		}
	}
	else
	{
		if (m_nCursorPos < (int)(m_sInputBuffer.length()))
		{
			const char* const pBase   = m_sInputBuffer.c_str();
			const char*       pCursor = pBase + m_nCursorPos;
			const char* const pBegin  = pCursor;
#if 0
			Unicode::CIterator<const char*, false> pUnicode(pCursor);
			pUnicode--; // Remove one UCS code-point, doesn't account for combining diacritics
			pCursor = pUnicode.GetPosition();
			const size_t length = pCursor - pBegin;
#endif
			const size_t length = 1;
			m_sInputBuffer.erase(pBegin - pBase, length);
		}
	}
}

void CXConsole::ExecuteInputBuffer()
{
	string sTemp = m_sInputBuffer;
	if (m_sInputBuffer.empty())
		return;
	m_sInputBuffer = "";

	AddCommandToHistory(sTemp.c_str());

#if 0
	if (Env::System()->IsCVarWhitelisted(sTemp.c_str(), false))
#endif
	ExecuteStringInternal(sTemp.c_str(), true); // from console

	m_nCursorPos = 0;
}

void CXConsole::ExecuteCommand(CConsoleCommand& cmd, string& str, bool bIgnoreDevMode)
{
	CryLog("[CONSOLE] Executing console command '%s'", str.c_str());
	//INDENT_LOG_DURING_SCOPE();

	std::vector<string> args;

	const char*         start       = str.c_str();
	const char*         commandLine = start;
	while (char ch = *commandLine++)
	{
		switch (ch)
		{
		case '\'':
		case '\"':
		{
			while ((*commandLine++ != ch) && *commandLine)
				;
			args.push_back(string(start + 1, commandLine - 1));
			start = commandLine;
			break;
		}
		case ' ':
			start = commandLine;
			break;
		default:
		{
			if ((*commandLine == ' ') || !*commandLine)
			{
				args.push_back(string(start, commandLine));
				start = commandLine + 1;
			}
		}
		break;
		}
	}

	if (args.size() >= 2 && args[1] == "?")
	{
		DisplayHelp(cmd.m_sHelp.data(), cmd.m_sName.c_str());
		return;
	}

	if (((cmd.m_nFlags & (VF_CHEAT | VF_CHEAT_NOCHECK | VF_CHEAT_ALWAYS_CHECK)) != 0) && !(Env::Get()->IsEditor()))
	{
#if LOG_CVAR_INFRACTIONS
		Env::Log()->LogError("[CVARS]: [EXECUTE] command %s is marked [VF_CHEAT]", cmd.m_sName.c_str());
	#if LOG_CVAR_INFRACTIONS_CALLSTACK
		Env::System()->debug_LogCallStack();
	#endif
#endif
		if (!(Env::Get()->IsEditor()) && !(m_system.IsDevMode()) && !bIgnoreDevMode)
			return;
	}

	if (cmd.m_func)
	{
		// This is function command, execute it with a list of parameters.
		CConsoleCommandArgs cmdArgs(str, args);
		if (!cmd.m_isManagedExternally)
			cmd.m_func(&cmdArgs);
#if 0
		else
		{
			for (TManagedConsoleCommandListener::Notifier notifier(m_managedConsoleCommandListeners); notifier.IsValid(); notifier.Next())
				notifier->OnManagedConsoleCommandEvent(cmd.m_sName.c_str(), &cmdArgs);
		}
#endif

		return;
	}

	string buf;
	{
		// only do this for commands with script implementation
		size_t t = 1;
		for (;;)
		{
			t = str.find_first_of("\\", t);
			if (t == string::npos)
				break;
			str.replace(t, 1, "\\\\", 2);
			t += 2;
		}

		for (t = 1;;)
		{
			t = str.find_first_of("\"", t);
			if (t == string::npos)
				break;
			str.replace(t, 1, "\\\"", 2);
			t += 2;
		}

		buf       = cmd.m_sCommand;

		size_t pp = buf.find("%%");
		if (pp != string::npos)
		{
			string list = "";
			for (unsigned int i = 1; i < args.size(); i++)
			{
				list += "\"" + args[i] + "\"";
				if (i < args.size() - 1) list += ",";
			}
			buf.replace(pp, 2, list);
		}
		else if ((pp = buf.find("%line")) != string::npos)
		{
			string tmp = "\"" + str.substr(str.find(" ") + 1) + "\"";
			if (args.size() > 1)
			{
				buf.replace(pp, 5, tmp);
			}
			else
			{
				buf.replace(pp, 5, "");
			}
		}
		else
		{
			for (unsigned int i = 1; i <= args.size(); i++)
			{
				char pat[10];
				sprintf(pat, "%%%u", i);
				size_t pos = buf.find(pat);
				if (pos == string::npos)
				{
					if (i != args.size())
					{
						ConsoleWarning("Too many arguments for: %s", cmd.m_sName.c_str());
						return;
					}
				}
				else
				{
					if (i == args.size())
					{
						ConsoleWarning("Not enough arguments for: %s", cmd.m_sName.c_str());
						return;
					}
					string arg = "\"" + args[i] + "\"";
					buf.replace(pos, strlen(pat), arg);
				}
			}
		}
	}

	if (m_system.GetIScriptSystem())
		m_system.GetIScriptSystem()->ExecuteBuffer(buf.c_str(), buf.length());
	m_bDrawCursor = 0;
}

void CXConsole::ScrollConsole()
{
	if (!m_pRenderer)
		return;

	//int nCurrHeight = m_pRenderer->GetOverlayHeight();
	int nCurrHeight = m_pRenderer->GetHeight();

	switch (m_sdScrollDir)
	{
	case sdDOWN:
		m_nScrollPos = m_nTempScrollMax;

		if (m_nScrollPos > m_nTempScrollMax)
		{
			m_nScrollPos  = m_nTempScrollMax;
			m_sdScrollDir = sdNONE;
		}
		break;
	case sdUP:
		m_nScrollPos -= nCurrHeight; //2;

		if (m_nScrollPos < 0)
		{
			m_nScrollPos  = 0;
			m_sdScrollDir = sdNONE;
		}
		break;
	case sdNONE:
		break;
	}
}

void CXConsole::ConsoleLogInputResponse(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Env::Log()->LogV(ILog::eInputResponse, format, args);
	va_end(args);
}

void CXConsole::ConsoleLogInput(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Env::Log()->LogV(ILog::eInput, format, args);
	va_end(args);
}

void CXConsole::ConsoleWarning(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Env::Log()->LogV(ILog::eWarningAlways, format, args);
	va_end(args);
}

void CXConsole::DisplayHelp(const char* help, const char* name)
{
	if (help == 0 || *help == 0)
	{
		ConsoleLogInputResponse("No help available for $3%s", name);
	}
	else
	{
		char *start, *pos;
		for (pos = (char*)help, start = (char*)help;
		     (pos = strstr(pos, "\n")) != nullptr;
		     start = ++pos)
		{
			string s = start;
			s.resize(pos - start);
			ConsoleLogInputResponse("    $3%s", s.c_str());
		}
		ConsoleLogInputResponse("    $3%s", start);
	}
}

void CXConsole::DisplayVarValue(ICVar* pVar)
{
	if (!pVar)
		return;

	const char* sFlagsString    = GetFlagsString(pVar->GetFlags());
	string      sValue          = (pVar->GetFlags() & VF_INVISIBLE) ? "" : pVar->GetString();
	string      sVar            = pVar->GetName();

	char        szRealState[40] = "";
	if (ECVarType(pVar->GetType()) == ECVarType::Int)
	{
#if 0
		const int iRealState = pVar->GetRealIVal();
#else
		const int iRealState = pVar->GetIVal();
#endif
		if (iRealState != pVar->GetIVal())
		{
			if (iRealState == -1)
				strcpy(szRealState, " RealState=Custom");
			else
				sprintf(szRealState, " RealState=%d", iRealState);
		}
	}

	if (pVar->GetFlags() & VF_BITFIELD)
	{
		assert(0);
#if 0
		const uint64 val64 = pVar->GetI64Val();
		const uint64 alphaBits = val64 & ~63LL;
		const uint32 nonAlphaBits = val64 & 63;

		if (alphaBits != 0)
		{
			// the bottom 6 bits can't be set by char entry, so show them separately
			char alphaChars[65];  // 1 char per bit + '\0'
			BitsAlpha64(alphaBits, alphaChars);
			sValue += " (";
			if (nonAlphaBits != 0)
			{
				char nonAlphaChars[3];  // 1..63 + '\0'
				sValue += itoa(nonAlphaBits, nonAlphaChars, 10);
				sValue += ", ";
			}
			sValue += alphaChars;
			sValue += ")";
		}
#endif
	}

	if (Env::Get()->IsEditor())
		ConsoleLogInputResponse("%s=%s [ %s ]%s", sVar.c_str(), sValue.c_str(), sFlagsString, szRealState);
	else
		ConsoleLogInputResponse("    $3%s = $6%s $5[%s]$4%s", sVar.c_str(), sValue.c_str(), sFlagsString, szRealState);
}

void CXConsole::SplitCommands(const char* line, std::list<string>& split)
{
	const char* start = line;
	string      working;

	while (true)
	{
		char ch = *line++;
		switch (ch)
		{
		case '\'':
		case '\"':
			while ((*line++ != ch) && *line)
				;
			break;
		case '\n':
		case '\r':
		case ';':
		case '\0':
		{
			working.assign(start, line - 1);
			working = Trim(working);

			if (!working.empty())
				split.push_back(working);
			start = line;

			if (ch == '\0')
				return;
		}
		break;
		}
	}
}

IFont* CXConsole::GetFont(const char* name, float w, float h)
{
	if (Env::Get()->IsDedicated())
		m_pFont = new CNullFont();
	else
	{
		m_pFont   = Env::Renderer()->GetIFont();
		auto font = name;
		auto var  = GetCVar("s_font");
		if (var)
			font = var->GetString();
		m_pFont->Init(font, (int)con_font_size, (int)con_font_size);
		//return m_pFont;
	}
	return m_pFont;
}

#pragma endregion
