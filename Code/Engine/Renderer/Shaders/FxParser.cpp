#include <BlackBox/Core/Platform/CryLibrary.h>

#include "Driver.hpp"
#include "FxParser.h"

#include <memory>

HMODULE	 m_FxLibrary{};
IDriver* g_driver{};

FxParser::FxParser()
{
}

FxParser::~FxParser()
{
	SAFE_RELEASE(g_driver);
	CryFreeLibrary(m_FxLibrary);
}

bool FxParser::Parse(const std::string& f, PEffect* pEffect)
{
#ifndef _LIB
#	define DLL_MODULE_INIT_ISYSTEM "ModuleInitISystem"
	//auto g_driver = std::unique_ptr<IDriver>(CreateParserDriver());
	//CCryLibrary ;
	if (m_FxLibrary = CryLoadLibrary("FxParser"))
	{
		PFNCREATEDRIVERINTERFACE CreateParserDriver = (PFNCREATEDRIVERINTERFACE)CryGetProcAddress(m_FxLibrary, "CreateParserDriver");
		g_driver									= CreateParserDriver();

		typedef void* (*PtrFunc_ModuleInitISystem)(ISystem * pSystem, const char* moduleName);
		PtrFunc_ModuleInitISystem pfnModuleInitISystem = (PtrFunc_ModuleInitISystem)CryGetProcAddress(m_FxLibrary, DLL_MODULE_INIT_ISYSTEM);
		if (pfnModuleInitISystem)
		{
			pfnModuleInitISystem(gEnv->pSystem, "FxParser");
		}
		*pEffect = g_driver->parse(f.data());
		return *pEffect != nullptr;
	}
	return false;
#else
	g_driver = CreateParserDriver();
	*pEffect = g_driver->parse(f.data());
	return *pEffect != nullptr;
#endif // !STATIC_PARSER
}
