﻿#pragma once
#include <BlackBox/Render/Opengl.hpp>
#include <iostream>
#include <fstream>


#define TO_STR(v, m) case GL_##v: { return m"GL_"#v; }
#define GET_SOURCE(v) TO_STR(v, "Source: ")
#define GET_TYPE(v) TO_STR(v, "Type: ")
#define GET_SEVERITY(v) TO_STR(v, "Severity: ")

class OpenglDebug
{
public:
	OpenglDebug(const char *file);
	~OpenglDebug();

	static void checkError(const char *file, int line, const char *expr);
private:
	static void APIENTRY callBack(GLenum source​, GLenum type​, GLuint id​,
	GLenum severity​, GLsizei length​, const GLchar* message​, const void* userParam​);

	static inline const char* SOURCE_TO_STRING(GLenum s)
	{
		switch (s)
		{
			GET_SOURCE(DEBUG_SOURCE_API);
			GET_SOURCE(DEBUG_SOURCE_WINDOW_SYSTEM);
			GET_SOURCE(DEBUG_SOURCE_SHADER_COMPILER);
			GET_SOURCE(DEBUG_SOURCE_THIRD_PARTY);
			GET_SOURCE(DEBUG_SOURCE_APPLICATION);
			GET_SOURCE(DEBUG_SOURCE_OTHER);
		default:
			return "Unknown source";
		}
	}

	static inline const char* TYPE_TO_STRING(GLenum t)
	{
		switch (t)
		{
			GET_TYPE(DEBUG_TYPE_ERROR);
			GET_TYPE(DEBUG_TYPE_DEPRECATED_BEHAVIOR);
			GET_TYPE(DEBUG_TYPE_UNDEFINED_BEHAVIOR);
			GET_TYPE(DEBUG_TYPE_PORTABILITY);
			GET_TYPE(DEBUG_TYPE_PERFORMANCE);
			GET_TYPE(DEBUG_TYPE_MARKER);
			GET_TYPE(DEBUG_TYPE_PUSH_GROUP);
			GET_TYPE(DEBUG_TYPE_POP_GROUP);
			GET_TYPE(DEBUG_TYPE_OTHER);
		default:
			return "Unknown type";
		}
	}

	static inline const char* SEVERITY_TO_STRING(GLenum s)
	{
		switch (s)
		{
			GET_SEVERITY(DEBUG_SEVERITY_HIGH);
			GET_SEVERITY(DEBUG_SEVERITY_MEDIUM);
			GET_SEVERITY(DEBUG_SEVERITY_LOW);
			GET_SEVERITY(DEBUG_SEVERITY_NOTIFICATION);
		default:
			return "Unknown severity";
		}
	}

private:
	std::ofstream debug_file;
	static bool isError;
};

#ifdef _DEBUG

// In debug mode, perform a test on every OpenGL call
// The do-while loop is needed so that glCheck can be used as a single statement in if/else branches
#define glCheck(expr) do { expr; OpenglDebug::checkError(__FILE__, __LINE__, #expr); } while (false)

#else

// Else, we don't add any overhead
#define glCheck(expr) (expr)

#endif