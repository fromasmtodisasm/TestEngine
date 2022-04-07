#pragma once

#if defined(IS_MONOLITHIC_BUILD)
	#define SYS_ENV_AS_STRUCT
#endif

#if defined(RC_COMPILER)
	#if (defined(_DEBUG) || defined(USE_DEBUG_NEW)) && !defined(LINUX)
		#include <crtdbg.h>
		#define DEBUG_CLIENTBLOCK new (_NORMAL_BLOCK, __FILE__, __LINE__)
	    #define new DEBUG_CLIENTBLOCK
		#define DEBUG_NEW         new 
		#define DEBUG_NEW_ARRAY   new[](_NORMAL_BLOCK, __FILE__, __LINE__)
	#endif
#endif
