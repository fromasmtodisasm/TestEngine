#include <BlackBox/Core/Platform/Windows.hpp>
#include <BlackBox/Core/Platform/platform_impl.inl>
#include <BlackBox/System/System.hpp>

#include <BlackBox/Core/IGame.hpp>
#include <BlackBox/Profiler/Drawer2D.h>
#include <BlackBox/Profiler/Profiler.h>
#include <BlackBox/Renderer/Camera.hpp>
#include <BlackBox/3DEngine/3DEngine.hpp>
#include <BlackBox/Scene/Scene.hpp>
#include <BlackBox/ScriptSystem/ScriptSystem.hpp>
#include <BlackBox/System/Console.hpp>
#include "RemoteConsole/RemoteConsole.h"
#include <BlackBox/System/IProjectManager.hpp>

#include <BlackBox/Core/Stream.hpp>
#include <BlackBox/System/HardwareMouse.hpp>
#include <BlackBox/System/IWindow.hpp>
#include <BlackBox/System/SystemEventDispatcher.hpp>
#include <BlackBox/System/VersionControl.hpp>
#include <BlackBox/World/IWorld.hpp>
#include <BlackBox/System/CVarOverrides.h>
#include <BlackBox/System/ConsoleRegistration.h>
//#include <BlackBox/Profiler/HP_Timer.h>
#include <SDL2/SDL.h>

#include "WindowsConsole.h"

#include <XML/xml.h>


// Define global cvars.
SSystemCVars g_cvars;

namespace utils
{
	void touch(IConsoleCmdArgs* args)
	{
		for (int i = 1; i < args->GetArgCount(); i++)
		{
			if (auto f = fopen(args->GetArg(i), "a"); f)
			{
				fclose(f);
				continue;
			}
			gEnv->pLog->LogError("Cannot touch file \"%s\"", args->GetArg(i));
		}
	}
}

void CSystem::SetGCFrequency(const float fRate)
{
}
void CSystem::SetIProcess(IProcess* process)
{
	m_pProcess = process;
}
IProcess* CSystem::GetIProcess()
{
	return m_pProcess;
}


CSystem::CSystem(SSystemInitParams& startupParams)
	:
#if defined(SYS_ENV_AS_STRUCT)
	  m_env(m_env),
#endif
	  //m_startupParams(startupParams),
	  cvGameName(nullptr),
	  //m_pInput(nullptr),
	  //m_pFont(nullptr),
	  m_pGame(nullptr),
	  m_pWindow(nullptr),
	  m_ScriptObjectConsole(nullptr),
	  m_pTextModeConsole(nullptr)
#if ENABLE_DEBUG_GUI
#endif
{
	m_startupParams			 = startupParams;
	m_pSystemEventDispatcher = new CSystemEventDispatcher(); // Must be first.
	if (m_pSystemEventDispatcher)
	{
		m_pSystemEventDispatcher->RegisterListener(this, "CSystem");
	}
	//////////////////////////////////////////////////////////////////////////
	// Initialize global environment interface pointers.
	m_env.pSystem = this;
	m_env.pTimer = &m_Time;

#if !defined(SYS_ENV_AS_STRUCT)
	gEnv = &m_env;
#endif
	m_pValidator = nullptr;

	m_env.pConsole = new CXConsole(*this);
	REGISTER_COMMAND("touch", utils::touch, 0, "touch file");
	if (startupParams.pPrintSync)
		m_env.pConsole->AddOutputPrintSink(startupParams.pPrintSync);
	InitThreadSystem();
}

CSystem::~CSystem()
{
	ShutDown();
}

void CSystem::Start()
{
	bool bRelaunch = m_env.IsDedicated();

	m_pGame->Run(bRelaunch);

	NOW	 = SDL_GetPerformanceCounter();
	LAST = 0;

	m_DeltaTime = 0.0;

	while (bRelaunch)
	{
		m_pGame->Release();
		m_pGame = CreateGame(nullptr);
		if (!m_pGame->Init(this, m_env.IsDedicated(), m_startupParams.bEditor, "Normal"))
			break;
		m_pGame->Run(bRelaunch);
	}
}

void CSystem::Release()
{
	delete this;
}

void CSystem::Relaunch(bool bRelaunch)
{
}

void CSystem::Error(const char* sFormat, ...)
{
	va_list	ArgList;
	char szBuffer[MAX_WARNING_LENGTH];
	va_start(ArgList, sFormat);
	vsprintf(szBuffer, sFormat, ArgList);
	va_end(ArgList);
	gEnv->pLog->LogError(szBuffer);
}

void CSystem::Warning(EValidatorModule module, EValidatorSeverity severity, int flags, const char* file, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	WarningV(module, severity, flags, file, format, args);
	va_end(args);
}

inline const char* ValidatorModuleToString(EValidatorModule module)
{
	switch (module)
	{
	case VALIDATOR_MODULE_RENDERER:
		return "Renderer";
	case VALIDATOR_MODULE_3DENGINE:
		return "3DEngine";
	case VALIDATOR_MODULE_ASSETS:
		return "Assets";
	case VALIDATOR_MODULE_AI:
		return "AI";
	case VALIDATOR_MODULE_ANIMATION:
		return "Animation";
	case VALIDATOR_MODULE_ENTITYSYSTEM:
		return "EntitySystem";
	case VALIDATOR_MODULE_SCRIPTSYSTEM:
		return "Script";
	case VALIDATOR_MODULE_SYSTEM:
		return "System";
	case VALIDATOR_MODULE_AUDIO:
		return "Audio";
	case VALIDATOR_MODULE_GAME:
		return "Game";
	case VALIDATOR_MODULE_MOVIE:
		return "Movie";
	case VALIDATOR_MODULE_EDITOR:
		return "Editor";
	case VALIDATOR_MODULE_NETWORK:
		return "Network";
	case VALIDATOR_MODULE_PHYSICS:
		return "Physics";
	case VALIDATOR_MODULE_FLOWGRAPH:
		return "FlowGraph";
	case VALIDATOR_MODULE_ONLINE:
		return "Online";
	case VALIDATOR_MODULE_DRS:
		return "DynamicResponseSystem";
	default:
		break;
	}
	return "";
}

void CSystem::WarningV(EValidatorModule module, EValidatorSeverity severity, int flags, const char* file, const char* format, va_list args)
{
	// Fran: No logging in a testing environment
	if (m_env.pLog == 0)
	{
		return;
	}

	#if 0
	const char* sModuleFilter = m_env.pLog->GetModuleFilter();
	if (sModuleFilter && *sModuleFilter != 0)
	{
		const char* sModule = ValidatorModuleToString(module);
		if (strlen(sModule) > 1 || CryStringUtils::stristr(sModule, sModuleFilter) == 0)
		{
			// Filter out warnings from other modules.
			return;
		}
	}
	#endif

	bool bDbgBreak = false;
	if (severity == VALIDATOR_ERROR_DBGBRK)
	{
		bDbgBreak = true;
		severity = VALIDATOR_ERROR; // change it to a standard VALIDATOR_ERROR for simplicity in the rest of the system
	}

	IMiniLog::ELogType ltype = ILog::eComment;
	switch (severity)
	{
	case VALIDATOR_ERROR:
		ltype = ILog::eError;
		break;
	case VALIDATOR_WARNING:
		ltype = ILog::eWarning;
		break;
	case VALIDATOR_COMMENT:
		ltype = ILog::eComment;
		break;
	case VALIDATOR_ASSERT:
		ltype = ILog::eAssert;
		break;
	default:
		break;
	}
	char szBuffer[MAX_WARNING_LENGTH];
	vsprintf(szBuffer, format, args);

	if (file && *file)
	{
		string fmt = szBuffer;
		fmt += " [File=";
		fmt += file;
		fmt += "]";

		m_env.pLog->LogWithType(ltype, flags | VALIDATOR_FLAG_SKIP_VALIDATOR, "%s", fmt.c_str());
	}
	else
	{
		m_env.pLog->LogWithType(ltype, flags | VALIDATOR_FLAG_SKIP_VALIDATOR, "%s", szBuffer);
	}

	//if(file)
	//m_env.pLog->LogWithType( ltype, "  ... caused by file '%s'",file);

	#if 0
	if (m_pValidator && (flags & VALIDATOR_FLAG_SKIP_VALIDATOR) == 0)
	{
		SValidatorRecord record;
		record.file = file;
		record.text = szBuffer;
		record.module = module;
		record.severity = severity;
		record.flags = flags;
		record.assetScope = m_env.pLog->GetAssetScopeString();
		m_pValidator->Report(record);
	}
	#endif

#if !defined(_RELEASE)
	if (bDbgBreak && g_cvars.sys_error_debugbreak)
		__debugbreak();
#endif

}

bool CSystem::CheckLogVerbosity(int verbosity)
{
	//FIXME: fix
	//return false;
	return true;
}

bool CSystem::IsQuitting()
{
	return m_bQuit;
}

void CSystem::Quit()
{
	//CryLog("CSystem::Quit invoked from thread %" PRI_THREADID " (main is %" PRI_THREADID ")", GetCurrentThreadId(), gEnv->mMainThreadId);

	if (m_bQuit)
		return;
	m_bQuit = true;

	// clean up properly the console
	if (m_pTextModeConsole)
		m_pTextModeConsole->OnShutdown();

	m_pSystemEventDispatcher->OnSystemEvent(ESYSTEM_EVENT_FAST_SHUTDOWN, 0, 0);

	GetIRemoteConsole()->Stop();

    extern std::vector<const char*> g_moduleCVars;
    printf("vars size: %d", g_moduleCVars.size());
	#if 0
    for (auto& var : g_moduleCVars)
    {
        printf("var: %s", var);
    }
	#endif

	//Release();

	
	if (0)
	{
#if BB_PLATFORM_WINDOWS
		TerminateProcess(GetCurrentProcess(), 0);
#else
		_exit(0);
#endif
	}
#if !BB_PLATFORM_LINUX && !BB_PLATFORM_ANDROID && !BB_PLATFORM_APPLE && !BB_PLATFORM_DURANGO && !BB_PLATFORM_ORBIS
	//PostQuitMessage(0);
#endif

}

IFont* CSystem::GetIFont()
{
	assert(0);
	return nullptr;
	//return new FreeTypeFont;
}

IWindow* CSystem::GetIWindow()
{
    return m_pWindow;
}

#if 0
IInputHandler* CSystem::GetIInputHandler()
{
  return m_InputHandler;
}
#endif

bool CSystem::ConfigLoad(const char* file)
{
	//m_pConsole->ExecuteFile(file);
	LoadConfiguration(file);

	return true;
}

void CSystem::RunMainLoop()
{
	for (;;)
	{
		if (!DoFrame())
		{
			break;
		}
	}
}

bool CSystem::DoFrame(int updateFlags)
{
	bool continueRunning = true;
	int pauseMode{};

	if (m_env.pFrameProfileSystem)
		m_env.pFrameProfileSystem->StartFrame();

	if (!m_env.IsEditing()) // Editor calls its own rendering update
		RenderBegin();

	if (!Update(updateFlags, pauseMode))
	{
		continueRunning = false;
	}

	m_pGame->Update();

	Render();
	RenderEnd();

	if (ITextModeConsole* pTextModeConsole = GetITextModeConsole())
	{
		pTextModeConsole->EndDraw();
	}

	if (m_env.pFrameProfileSystem)
		m_env.pFrameProfileSystem->EndFrame();

	SleepIfNeeded();
	return continueRunning;
}

void CSystem::ParseCMD()
{
	std::string cmd = m_startupParams.szSystemCmdLine;
	if (cmd.find("-nsightDebug") != std::string::npos)
	{
		m_env.pConsole->CreateVariable("nsightDebug", 1, VF_NULL, "Debuggin via Nsight Graphics");
	}
}

void CSystem::LoadScreen()
{
	if (m_env.IsDedicated())
	{
		return;
	}
	//m_pConsole->Clear();
	m_env.pConsole->SetScrollMax(300);
	m_env.pConsole->ShowConsole(false);

	string sLoadingScreenTexture = string("loading.png");

	m_env.pConsole->SetLoadingImage(sLoadingScreenTexture.c_str());
	#if 0
	m_env.pConsole->ResetProgressBar(0x7fffffff);
	GetILog()->UpdateLoadingScreen("");	// just to draw the console
	#endif
}

bool CSystem::InitScripts()
{
	CryLog("Initializing Script Bindings");

	m_ScriptObjectConsole = new CScriptObjectConsole();
	CScriptObjectConsole::InitializeTemplate(m_env.pScriptSystem);

	m_ScriptObjectScript = new CScriptObjectScript();
	CScriptObjectScript::InitializeTemplate(m_env.pScriptSystem);

	m_ScriptObjectRenderer = new CScriptObjectRenderer();
	CScriptObjectRenderer::InitializeTemplate(m_env.pScriptSystem);

	m_ScriptObjectSound = new CScriptObjectSound();
	CScriptObjectSound::InitializeTemplate(m_env.pScriptSystem);

	m_ScriptObjectConsole->Init(GetIScriptSystem(), m_env.pConsole);
	m_ScriptObjectScript->Init(GetIScriptSystem());
	m_ScriptObjectSound->Init(GetIScriptSystem());
	return m_env.pScriptSystem->ExecuteFile("scripts/engine.lua");
}

void CSystem::ReleaseScripts()
{
	CScriptObjectConsole::ReleaseTemplate();
	CScriptObjectScript::ReleaseTemplate();
	CScriptObjectRenderer::ReleaseTemplate();
}

void CSystem::SetWorkingDirectory(const std::string& path) const
{
	fs::current_path(fs::path(path));
}

void CSystem::LogCommandLine() const
{
	std::cout << "Log command line" << std::endl;
	for (int i = 0; i < m_pCmdLine->GetArgCount(); i++)
	{
		CryLog("CMD: %s", m_pCmdLine->GetArg(i)->GetValue());
	}
}

void CSystem::Tests()
{
#if 0
	auto plane_mesh = CreatePlane(4, 4);
	mesh = std::make_shared<std::vector<Mesh>>();
	mesh->push_back(*plane_mesh.get());
	obj = Object::getEmpty();
	obj->m_Mesh = mesh;
	obj->m_path = "";
#endif

	//SceneManager::instance()->currentScene()->addObject("subdiveded plane", obj);
	//Sleep(3000);
}

void CSystem::PollEvents()
{
}

void CSystem::CreateRendererVars(const SSystemInitParams& startupParams)
{
	m_rDriver = REGISTER_STRING("r_Driver", "DX11", VF_DUMPTODISK | VF_REQUIRE_APP_RESTART,
		"Sets the renderer driver ( DX11/DX12/GL/VK/AUTO ).\n"
		"Specify in system.cfg like this: r_Driver = \"DX11\"");
	REGISTER_CVAR2("r_InitialWindowSizeRatio", &m_rIntialWindowSizeRatio, 0.666f, VF_DUMPTODISK,
				   "Sets the size ratio of the initial application window in relation to the primary monitor resolution.\n"
				   "Usage: r_InitialWindowSizeRatio [1.0/0.666/..]");

	int iFullScreenDefault	= 0;
	int iDisplayInfoDefault = 1;
	int iWidthDefault		= 1280;
	int iHeightDefault		= 720;
#if BB_PLATFORM_WINDOWS && 0
	iFullScreenDefault				   = 0;
	const float initialWindowSizeRatio = m_rIntialWindowSizeRatio->GetFVal();
	iWidthDefault					   = static_cast<int>(GetSystemMetrics(SM_CXSCREEN) * initialWindowSizeRatio);
	iHeightDefault					   = static_cast<int>(GetSystemMetrics(SM_CYSCREEN) * initialWindowSizeRatio);
#elif BB_PLATFORM_LINUX || BB_PLATFORM_APPLE
	iFullScreenDefault		 = 0;
#endif

#if defined(RELEASE)
	iDisplayInfoDefault = 0;
#endif

	// load renderer settings from engine.ini
	REGISTER_CVAR2("r_Width", &m_rWidth, iWidthDefault, VF_DUMPTODISK,
				   "Sets the display width, in pixels.\n"
				   "Usage: r_Width [800/1024/..]");
	REGISTER_CVAR2("r_Height", &m_rHeight, iHeightDefault, VF_DUMPTODISK,
				   "Sets the display height, in pixels.\n"
				   "Usage: r_Height [600/768/..]");
	REGISTER_CVAR2("r_ColorBits", &m_rColorBits, 32, VF_DUMPTODISK | VF_REQUIRE_APP_RESTART,
				   "Sets the color resolution, in bits per pixel. Default is 32.\n"
				   "Usage: r_ColorBits [32/24/16/8]");
	REGISTER_CVAR2("r_DepthBits", &m_rDepthBits, 24, VF_DUMPTODISK | VF_REQUIRE_APP_RESTART,
				   "Sets the depth precision, in bits per pixel. Default is 24.\n"
				   "Usage: r_DepthBits [32/24/16]");
	REGISTER_CVAR2("r_StencilBits", &m_rStencilBits, 8, VF_DUMPTODISK,
				   "Sets the stencil precision, in bits per pixel. Default is 8.\n");

	REGISTER_CVAR2("r_Fullscreen", &m_rFullscreen, iFullScreenDefault, VF_DUMPTODISK,
				   "Toggles fullscreen mode. Default is 1 in normal game and 0 in DevMode.\n"
				   "Usage: r_Fullscreen [0=window/1=fullscreen]");

#if 0
	REGISTER_CVAR2("r_FullscreenNativeRes", &m_rFullscreenNativeRes, 0, VF_DUMPTODISK,
		"Toggles native resolution upscaling.\n"
		"If enabled, scene gets upscaled from specified resolution while UI is rendered in native resolution.");
#endif

	REGISTER_CVAR2("r_DisplayInfo", &m_rDisplayInfo, 1, VF_RESTRICTEDMODE | VF_DUMPTODISK,
				   "Toggles debugging information display.\n"
				   "Usage: r_DisplayInfo [0=off/1=show/2=enhanced/3=minimal/4=fps bar/5=heartbeat]");
	REGISTER_CVAR2("r_Debug", &m_rDebug, 0, VF_RESTRICTEDMODE | VF_DUMPTODISK,
				   "Toggles debugging of renderer.\n"
				   "Usage: r_DisplayInfo [0=off/1=on]");
	REGISTER_CVAR2("r_Tonemap", &m_rTonemap, 1, VF_DUMPTODISK,
				   "Using tonemap.\n"
				   "Usage: r_Tonemap [0=off/1=on]");
	REGISTER_CVAR2("r_SkipShaderCache", &m_rSkipShaderCache, 1, VF_DUMPTODISK,
				   "Skip loading binary shader from disk.\n"
				   "Usage: r_SkipShaderCache [0=off/1=on]");
}

void CSystem::CreateSystemVars()
{
	#define DEFAULT_SYS_MAX_FPS 60
	REGISTER_CVAR2("sys_dump_memstats", &sys_dump_memstats, 0, VF_NULL, "");
	REGISTER_CVAR2("sys_MaxFPS", &g_cvars.sys_MaxFPS, DEFAULT_SYS_MAX_FPS, VF_NULL, "Limits the frame rate to specified number n (if n>0 and if vsync is disabled).\n"
																					" 0 = on PC if vsync is off auto throttles fps while in menu or game is paused (default)\n"
																					"-1 = off");
}

void CSystem::ShutDown()
{
	CSystem::Log("Releasing system");

	if (m_pSystemEventDispatcher)
	{
		m_pSystemEventDispatcher->RemoveListener(this);
	}

	if (m_pUserCallback)
	{
		m_pUserCallback->OnShutdown();
		m_pUserCallback = nullptr;
	}

	GetIRemoteConsole()->Stop();
	GetIRemoteConsole()->UnregisterConsoleVariables();

	ShutDownThreadSystem();

	SAFE_DELETE(m_pTextModeConsole);

	SAFE_RELEASE(m_pGame);
	//SAFE_DELETE(m_pFont);
	SAFE_RELEASE(m_pWindow);
	SAFE_RELEASE(m_env.p3DEngine);
	SAFE_RELEASE(m_env.pRenderer);

	SAFE_DELETE(m_ScriptObjectConsole);
	SAFE_DELETE(m_ScriptObjectSound);
	SAFE_DELETE(m_ScriptObjectScript);
	SAFE_DELETE(m_ScriptObjectRenderer);
	ReleaseScripts();
	SAFE_RELEASE(m_env.pScriptSystem);

	SAFE_RELEASE(m_env.pHardwareMouse);
	if (m_env.pInput)
		m_env.pInput->ShutDown();
	SAFE_RELEASE(m_env.pEntitySystem);
	SAFE_RELEASE(m_pNetwork);
	SAFE_RELEASE(m_env.pConsole);
	SAFE_DELETE(m_pSystemEventDispatcher);
	SAFE_DELETE(m_pCmdLine);
	SAFE_DELETE(m_env.pProjectManager);
	SAFE_RELEASE(m_env.pLog);
	SAFE_RELEASE(m_env.pSoundSystem);
	SAFE_RELEASE(m_env.pCryPak);
	SAFE_RELEASE(m_pCryPak);
	UnloadSubsystems();
    SDL_Quit();
}

void CSystem::EnableGui(bool enable)
{
#if ENABLE_DEBUG_GUI
    if (enable)
    {
        m_env.pInput->AddEventListener(m_GuiManager);
    }
    else
    {
        m_env.pInput->RemoveEventListener(m_GuiManager);
    }
#endif
}

void CSystem::SaveConfiguration()
{
}

float CSystem::GetDeltaTime()
{
    return static_cast<float>(m_DeltaTime);
}

const SFileVersion& CSystem::GetFileVersion()
{
    return m_FileVersion;
}

const SFileVersion& CSystem::GetProductVersion()
{
    return m_ProductVersion;
}

const char * CSystem::GetRootFolder() const
{
	return m_RootFolder.c_str();
}

IEntitySystem* CSystem::GetIEntitySystem()
{
    return m_env.pEntitySystem;
}

ICryPak* CSystem::GetIPak()
{
	return m_env.pCryPak;
}

INetwork* CSystem::GetINetwork()
{
	return m_pNetwork;
}

ITimer* CSystem::GetITimer()
{
    return &m_Time;
}
void CSystem::SetForceNonDevMode(const bool bValue)
{
}

bool CSystem::GetForceNonDevMode() const
{
	return false;
}
bool CSystem::WasInDevMode() const
{
	return false;
}

bool CSystem::IsDevMode() const
{
	return true;
}

void CSystem::Error(const char* message)
{
}

void CSystem::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
	case ESYSTEM_EVENT_CHANGE_FOCUS:
		break;
	case ESYSTEM_EVENT_MOVE:
		break;
	case ESYSTEM_EVENT_RESIZE:
		m_rWidth  = (int)wparam;
		m_rHeight = (int)lparam;
		break;
	case ESYSTEM_EVENT_ACTIVATE:
		break;
	case ESYSTEM_EVENT_LEVEL_LOAD_START:
		break;
	case ESYSTEM_EVENT_LEVEL_GAMEPLAY_START:
		break;
	case ESYSTEM_EVENT_LEVEL_POST_UNLOAD:
		break;
	case ESYSTEM_EVENT_LANGUAGE_CHANGE:
		break;
	case ESYSTEM_EVENT_TOGGLE_FULLSCREEN:
		m_rFullscreen = (int)wparam;
		break;
	case ESYSTEM_EVENT_GAMEWINDOW_ACTIVATE:
		m_bIsActive = bool(wparam);
		if (m_bIsActive)
			g_cvars.sys_MaxFPS = 60; 
		else
			g_cvars.sys_MaxFPS = 20; 
		break;
	default:
		break;
	}
}

void CSystem::ShowMessage(const char* message, const char* caption, MessageType messageType)
{
#ifdef _WIN32
	::MessageBox(NULL, message, caption, messageType == 0 ? MB_OK : MB_OKCANCEL);
#endif
}

void CSystem::Log(const char* message)
{
	//std::cout << "-- " << message << std::endl;
	gEnv->pLog->Log("%s", message);
}

IScriptSystem* CSystem::GetIScriptSystem()
{
	//2841004695
	return m_env.pScriptSystem;
}

bool CSystem::OnBeforeVarChange(ICVar* pVar, const char* sNewValue)
{
	if (!strcmp(pVar->GetName(), "r_cap_profile"))
	{
		switch (std::atoi(sNewValue))
		{
		case 0:
			PROFILER_UNFROZE_FRAME();
			return false;
		case 1:
			PROFILER_FROZE_FRAME();
			return false;
		default:
			return true;
		}
	}
	return true;
}

bool CSystem::OnInputEvent(const SInputEvent& event)
{
	bool result = false;
	//TODO: handle resized
	bool resized = false;
	switch (event.deviceType)
	{
	case eIDT_Mouse:
	{
		bool mouseMoved = event.keyId == eKI_MouseX || event.keyId == eKI_MouseY || event.keyId == eKI_MouseZ;
		if (event.state == eIS_Pressed)
		{
			if (event.keyId == eKI_Mouse1)
			{
				PROFILER_ON_LEFT_CLICK();
			}
			break;
		}
		if (mouseMoved)
		{
			//TODO: IMPLEMENT THIS
#if 0
      PROFILER_ON_MOUSE_POS(event.mouseMove.x, event.mouseMove.y);
#endif
			break;
		}
	}
	case eIDT_Keyboard:
	{
		if (event.state == eIS_Pressed)
		{
			if (event.modifiers == eMM_Alt && event.modifiers == eMM_Shift)
			{
				if (event.keyId == eKI_P)
				{
					if (PROFILER_ISFROZEN())
					{
						PROFILER_UNFROZE_FRAME();
					}
					else
					{
						PROFILER_FROZE_FRAME();
					}
				}
			}
			else if (event.modifiers == eMM_Alt)
			{
				if (event.keyId == eKI_Enter)
				{
					//TODO: IMPLEMENT THIS
#if 0
          static_cast<CWindow*>(m_pWindow)->ToogleFullScreen(1366, 768);
#endif
				}
			}
		}
	}
	default:
		break;
	}
	if (resized)
	{
		assert(0 && "Not implemented");
#if 0
    PROFILER_ON_RESIZE(event.size.width, event.size.height);
#endif
	}

	return result;
}

bool CSystem::Update(int updateFlags /* = 0*/, int nPauseMode /* = 0*/)
{
	FUNCTION_PROFILER(PROFILE_SYSTEM)

	m_env.pTimer->UpdateOnFrameStart();
	LAST = NOW;
	NOW	 = SDL_GetPerformanceCounter();

	//m_pNetwork->Update();
	if (nPauseMode)
	{
#if ENABLE_DEBUG_GUI
		m_env.pInput->AddEventListener(m_GuiManager);
#endif
	}

	if (m_pUserCallback)
		m_pUserCallback->OnUpdate();

	m_pSystemEventDispatcher->Update();
#if !defined(RELEASE) || defined(RELEASE_LOGGING)
	GetIRemoteConsole()->Update();
#endif

	//check if we are quitting from the game
	if (IsQuitting())
		return (false);

	m_DeltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency()) * 0.001;
	{
		PROFILER_PUSH_CPU_MARKER("INPUT", Utils::COLOR_LIGHT_BLUE);
		//FIXME: CHECK IT
		if (m_env.pInput)
			m_env.pInput->Update(true);
		PROFILER_POP_CPU_MARKER();
	}
	if (m_pWindow)
		m_pWindow->update();

	if (m_env.pConsole)
		m_env.pConsole->Update();
	if (m_pNetwork)
		m_pNetwork->UpdateNetwork();
	if (m_pWindow && m_pWindow->closed())
	{
		m_pGame->SendMessage("Quit");
	}
	if (!nPauseMode)
	{
		m_env.p3DEngine->Update();
	}

	//m_env.pScriptSystem->update

	if (m_env.pInput && m_env.pInput->GetModifiers() & eMM_Ctrl)
	{
		return false;
	}

	return !m_bQuit;
}

bool CSystem::WriteCompressedFile(char* filename, void* data, unsigned int bitlen)
{
	FILE* fp	= fopen(filename, "wb");
	bool result = false;
	if (fp != nullptr)
	{
		if (BITS2BYTES(bitlen) == fwrite(data, 1, BITS2BYTES(bitlen), fp))
		{
			result = true;
		}
		fclose(fp);
	}
	return result;
}

unsigned int CSystem::ReadCompressedFile(char* filename, void* data, unsigned int maxbitlen)
{
	FILE* fp   = fopen(filename, "rb");
	int result = 0;
	if (fp != nullptr)
	{
		result = fread(data, 1, BITS2BYTES(maxbitlen), fp);
		fclose(fp);
	}
	return result;
}

unsigned int CSystem::GetCompressedFileSize(char* filename)
{
	FILE* fp = fopen(filename, "rb");
	int size = 0;
	if (fp != nullptr)
	{
		fseek(fp, 0L, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		fclose(fp);
	}
	return BYTES2BITS(size);
}

void CSystem::OnAfterVarChange(ICVar* pVar)
{
}

void CSystem::OnVarUnregister(ICVar* pVar)
{
}

void CSystem::RenderStatistics()
{
}

const char* CSystem::GetUserName()
{
#if BB_PLATFORM_WINDOWS
	static const int iNameBufferSize = 1024;
	static char szNameBuffer[iNameBufferSize];
	memset(szNameBuffer, 0, iNameBufferSize);

	DWORD dwSize = iNameBufferSize;
	wchar_t nameW[iNameBufferSize];
	::GetUserNameW(nameW, &dwSize);
	strcpy(szNameBuffer, wstr_to_str(nameW).c_str());
	return szNameBuffer;
#elif BB_PLATFORM_LINUX || BB_PLATFORM_ANDROID
	static uid_t uid		 = geteuid();
	static struct passwd* pw = getpwuid(uid);
	if (pw)
	{
		return (pw->pw_name);
	}
	else
	{
		return NULL;
	}
#elif BB_PLATFORM_APPLE
	static const int iNameBufferSize = 1024;
	static char szNameBuffer[iNameBufferSize];
	if (AppleGetUserName(szNameBuffer, iNameBufferSize))
		return szNameBuffer;
	else
		return "";
#else
	return "";
#endif
}

void CSystem::FatalError(const char* format, ...)
{
	// format message
	va_list ArgList;
	char szBuffer[MAX_WARNING_LENGTH];
	const char* sPrefix = "";
	strcpy(szBuffer, sPrefix);
	va_start(ArgList, format);
	vsprintf(szBuffer + strlen(szBuffer)/*, sizeof(szBuffer) - strlen(szBuffer)*/, format, ArgList);
	va_end(ArgList);

	#
	// get system error message before any attempt to write into log
	const char* szSysErrorMessage = CryGetLastSystemErrorMessage();

	CryLogAlways("=============================================================================");
	CryLogAlways("*ERROR");
	CryLogAlways("=============================================================================");
	// write both messages into log
	CryLogAlways("%s", szBuffer);

	if (szSysErrorMessage)
		CryLogAlways("<System> Last System Error: %s", szSysErrorMessage);

	assert(szBuffer[0] >= ' ');
	//strcpy(szBuffer,szBuffer+1);	// remove verbosity tag since it is not supported by ::MessageBox

	//LogSystemInfo();

	//CollectMemStats(0, nMSP_ForCrashLog);

	OutputDebugString(szBuffer);
	//OnFatalError(szBuffer);

	if (!g_cvars.sys_no_crash_dialog)
	{
		CryMessageBox(szBuffer,"ENGINE FATAL ERROR", eMB_Error);
	}

	//GetITextModeConsole()->OnShutdown();
	DebugBreak();
	//__debugbreak();

}

//////////////////////////////////////////////////////////////////////////
void CSystem::ExecuteCommandLine()
{
	// should only be called once
	{
		static bool bCalledAlready = false;
		assert(!bCalledAlready);
		bCalledAlready = true;
	}

	// execute command line arguments e.g. +g_gametype ASSAULT +map "testy"

	ICmdLine* pCmdLine = GetICmdLine();
	assert(pCmdLine);

	const int iCnt = pCmdLine->GetArgCount();

	for (int i = 0; i < iCnt; ++i)
	{
		const ICmdLineArg* pCmd = pCmdLine->GetArg(i);

		if (pCmd->GetType() == eCLAT_Post)
		{
			string sLine = pCmd->GetName();

			if (gEnv->pSystem->IsCVarWhitelisted(sLine.c_str(), false))
			{
				if (pCmd->GetValue())
					sLine += string(" ") + pCmd->GetValue();

				GetILog()->Log("Executing command from command line: \n%s\n", sLine.c_str()); // - the actual command might be executed much later (e.g. level load pause)
				GetIConsole()->ExecuteString(sLine.c_str());
			}
#if defined(DEDICATED_SERVER)
			else
			{
				GetILog()->LogError("Failed to execute command: '%s' as it is not whitelisted\n", sLine.c_str());
			}
#endif
		}
	}

	//gEnv->pConsole->ExecuteString("sys_RestoreSpec test*"); // to get useful debugging information about current spec settings to the log file
}

//////////////////////////////////////////////////////////////////////////
bool CSystem::IsCVarWhitelisted(const char* szName, bool silent) const
{
	CRY_ASSERT(szName != nullptr);

	if (szName[0] == '?')
	{
		return true;
	}

	if (szName[0] == '+')
	{
		++szName;
	}

	const char* pNameEnd = std::max(strchr(szName, ' '), strchr(szName, '='));
	if (pNameEnd == nullptr)
	{
		return ::IsCVarWhitelisted(szName);
	}
	else
	{
		const string name(szName, pNameEnd);
		return ::IsCVarWhitelisted(name.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////
IStreamEngine* CSystem::GetStreamEngine()
{
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////
IRemoteConsole* CSystem::GetIRemoteConsole()
{
	return CRemoteConsole::GetInst();
}

ITextModeConsole* CSystem::GetITextModeConsole()
{
    if (m_env.IsDedicated())
		return m_pTextModeConsole;
	return 0;
}

IProjectManager* CSystem::GetIProjectManager()
{
	return m_env.pProjectManager;
}

LONG WINAPI MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionPtrs)
{
	static char msg[1024];

	CryError(
		"Code: %x\nAddress: 0x%p",
		pExceptionPtrs->ExceptionRecord->ExceptionCode,
		pExceptionPtrs->ExceptionRecord->ExceptionAddress
	);
	// Do something, for example generate error report
	MessageBox(NULL, msg, "Exception", 0);
	//..

	// Execute default exception handler next
	return EXCEPTION_EXECUTE_HANDLER;
}

int CSystem::GetCPUFlags()
{
	assert(0 && __FUNCTION__);
	return 0;
}
double CSystem::GetSecondsPerCycle()
{
	assert(0 && __FUNCTION__);
	return 0.;
}
void CSystem::DumpMemoryUsageStatistics() 
{

}
bool CSystem::IsTestMode() const
{
	assert(0 && __FUNCTION__);
	return false;
}
void CSystem::ShowDebugger(const char* pszSourceFile, int iLine, const char* pszReason)
{
	assert(0 && __FUNCTION__);
}
void CSystem::SetFrameProfiler(bool on, bool display, char* prefix)
{
	//assert(0 && __FUNCTION__);
}
void CSystem::StartProfilerSection(CFrameProfilerSection* pProfileSection)
{
	//assert(0 && __FUNCTION__);
}
void CSystem::EndProfilerSection(CFrameProfilerSection* pProfileSection)
{
	//assert(0 && __FUNCTION__);
}

void CSystem::SleepIfNeeded()
{
	FUNCTION_PROFILER(PROFILE_SYSTEM)

	static ICVar* pSysMaxFPS = NULL;
	static ICVar* pVSync	 = NULL;

	if (pSysMaxFPS == NULL && gEnv && gEnv->pConsole)
		pSysMaxFPS = gEnv->pConsole->GetCVar("sys_MaxFPS");
	if (pVSync == NULL && gEnv && gEnv->pConsole)
		pVSync = gEnv->pConsole->GetCVar("r_Vsync");

	int32 maxFPS = 0;

	#if 0
	if (m_env.IsDedicated())
	{
		const float maxRate = m_svDedicatedMaxRate->GetFVal();
		maxFPS				= int32(maxRate);
	}
	else
	#endif
	{
		if (pSysMaxFPS && pVSync)
		{
			uint32 vSync = pVSync->GetIVal();
			if (vSync == 0)
			{
				maxFPS = pSysMaxFPS->GetIVal();
				if (maxFPS == 0)
				{
					const bool bInLoading = true;	//(ESYSTEM_GLOBAL_STATE_RUNNING != m_systemGlobalState);
					if (bInLoading /* || IsPaused() || m_throttleFPS*/)
					{
						maxFPS = 60;
					}
				}
			}
		}
	}

	if (maxFPS > 0)
	{
		const int64 safeMarginMS = 5; // microseconds
		const int64 thresholdMs	 = (1000 * 1000) / (maxFPS);

		ITimer*		 pTimer		 = gEnv->pTimer;
		static int64 sTimeLast	 = pTimer->GetAsyncTime().GetMicroSecondsAsInt64();
		int64		 currentTime = pTimer->GetAsyncTime().GetMicroSecondsAsInt64();
		for (;;)
		{
			const int64 frameTime = currentTime - sTimeLast;
			if (frameTime >= thresholdMs)
				break;
			if (thresholdMs - frameTime > 10 * 1000)
				CrySleep(1);
			else
				CrySleep(0);

			currentTime = pTimer->GetAsyncTime().GetMicroSecondsAsInt64();
		}

		m_lastTickTime = pTimer->GetAsyncTime();
		sTimeLast	   = m_lastTickTime.GetMicroSecondsAsInt64() + safeMarginMS;
	}
}


ISystem* CreateSystemInterface(SSystemInitParams& initParams)
{
	std::unique_ptr<CSystem> pSystem = std::make_unique<CSystem>(initParams);
	initParams.pSystem				 = pSystem.get();
	ModuleInitISystem(pSystem.get(), "System");
#if BB_PLATFORM_WINDOWS
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
#endif

#if CRY_PLATFORM_DURANGO
#	if !defined(_LIB)
	m_env = pSystem->GetGlobalEnvironment();
#	endif
	m_env.pWindow = startupParams.hWnd;
#endif
	if (!pSystem->Init())
	{
		//pSystem.release();
		initParams.pSystem = nullptr;
		//gEnv->pSystem	   = nullptr;
		return nullptr;
	}
	pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_SYSTEM_INIT_DONE, 0, 0);
	// run main loop
	if (!initParams.bManualEngineLoop)
	{
		pSystem->RunMainLoop();
		return nullptr;
	}

	return pSystem.release();
}


XDOM::IXMLDOMDocument *CSystem::CreateXMLDocument()
{
	return new CXMLDocument;
}

XmlNodeRef CSystem::CreateXmlNode(const char *sNodeName)
{
	return {};
}
XmlNodeRef CSystem::LoadXmlFromString(const char *sXmlString)
{
	return {};

}

XmlNodeRef CSystem::LoadXmlFile(const char *sFilename)
{
	return {};
}

void CSystem::UpdateScriptSink()
{
	//PROFILE_SECTION(PROFILE_SCRIPT, "ScriptSystem: Update");
	//MEMSTAT_FUNCTION_CONTEXT(EMemStatContextType::Other);

	ITimer*	   pTimer	= gEnv->pTimer;
	CTimeValue nCurTime = pTimer->GetFrameStartTime();

	// Enable debugger if needed.
	// Some code is executed even if the debugMode doesn't change, including clearing the exposed stack
	#if 0
	ELuaDebugMode debugMode = (ELuaDebugMode)m_cvar_script_debugger->GetIVal();
	EnableDebugger(debugMode);
	#endif

	// Might need to check for new lua code needing hooks

	float currTime	= pTimer->GetCurrTime();
	float frameTime = pTimer->GetFrameTime();

	IScriptSystem* pScriptSystem = GetIScriptSystem();

	// Set global time variable into the script.
	pScriptSystem->SetGlobalValue("_time", currTime);
	pScriptSystem->SetGlobalValue("_frametime", frameTime);

	#if 0
	{
		int aiTicks = 0;

		IAISystem* pAISystem = gEnv->pAISystem;

		if (pAISystem)
			aiTicks = pAISystem->GetAITickCount();
		pScriptSystem->SetGlobalValue("_aitick", aiTicks);
	}

	//TRACE("GC DELTA %d",m_pScriptSystem->GetCGCount()-nStartGC);
	//int nStartGC = pScriptSystem->GetCGCount();

	bool bKickIn = false; // Invoke Gargabe Collector

	if (currTime - m_lastGCTime > m_fGCFreq) // g_GC_Frequence->GetIVal())
		bKickIn = true;

	int nGCCount = pScriptSystem->GetCGCount();

	bool bNoLuaGC = false;

	if (nGCCount - m_nLastGCCount > 2000 && !bNoLuaGC) //
		bKickIn = true;

	//if(bKickIn)
	{
		CRY_PROFILE_SECTION(PROFILE_SCRIPT, "Lua GC");

		//CryLog( "Lua GC=%d",GetCGCount() );

		//float fTimeBefore=pTimer->GetAsyncCurTime()*1000;

		// Do a full garbage collection cycle.
		//pScriptSystem->ForceGarbageCollection();

		// Do incremental Garbage Collection
		lua_gc(L, LUA_GCSTEP, (PER_FRAME_LUA_GC_STEP));

		m_nLastGCCount = pScriptSystem->GetCGCount();
		m_lastGCTime   = currTime;
		//float fTimeAfter=pTimer->GetAsyncCurTime()*1000;
		//CryLog("--[after coll]GC DELTA %d ",pScriptSystem->GetCGCount()-nGCCount);
		//TRACE("--[after coll]GC DELTA %d [time =%f]",m_pScriptSystem->GetCGCount()-nStartGC,fTimeAfter-fTimeBefore);
	}

	m_pScriptTimerMgr->Update(nCurTime.GetMilliSecondsAsInt64());
	#endif
}
