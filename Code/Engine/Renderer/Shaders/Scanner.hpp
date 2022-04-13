#pragma once

#if !defined(yyFlexLexerOnce)
	#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL yy::parser::symbol_type Scanner::ScanToken()
#include <stack>
#include <BlackBox/System/File/CryFile.h>

//#include "Parser.hpp"

class Driver;

class Scanner : public yyFlexLexer
{
public:
	Scanner(Driver& driver)
	    : driver(driver)
	{
		symboltype_map.insert({"float"});
		symboltype_map.insert({"float2"});
		symboltype_map.insert({"float3"});
		symboltype_map.insert({"float4"});

		symboltype_map.insert({"float4x4"});

		symboltype_map.insert({"int"});
		string_buf.reserve(1024 * 16);
	}
	virtual ~Scanner() {}
	int LexerInput(char* buf, int max_size) override
	{
		return ::yyFlexLexer::LexerInput(buf, max_size);
	}
	void LexerOutput(const char* buf, int size) override
	{
		CryLog("[Flexer] %s", buf);
	}
	void                            LexerError(const char* msg) override {
		CryFatalError("[Flexer] %s", msg);	
	}
	virtual yy::parser::symbol_type ScanToken();
	void                            pop_state() { yy_pop_state(); }
	void                            goto_codebody();
	bool                            MakeInclude(const char* file_name);
	void                            eof();

	bool                            register_type(const string& str)
	{
		symboltype_map.insert(str);
		return false;
	}
	void add_shader_fragment(const char* f)
	{
		defered_fragment = f + (" " + defered_fragment);
	}
	void add_shader_fragment_direct(const char* f)
	{
		shader += f;
	}
	void add_shader_fragment()
	{
		if (previewsCanAddFragment && canNowAddFragment)
		{
			shader += defered_fragment;
			defered_fragment = YYText();
		}
		else if (canNowAddFragment)
		{
			defered_fragment = YYText();
		}
		previewsCanAddFragment = canNowAddFragment;
	}
	yy::parser::symbol_type CurrentSymbol()
	{
		//return yy::parser::symbol_type(yy::parser::token::yytokentype(*YYText()), loc);
		return {};
	}

	void register_macro(std::string_view key, std::string_view value)
	{
		if (key.empty())
		{
			return;
		}
		macros[string(key)] = value;
		CryLog("Registered macro: %s", key.data());
	}

	yy::parser::symbol_type  check_type(const std::string&               s,
	                                    const yy::parser::location_type& loc);
	void                     print_state();

	Driver&                  driver;

	std::set<std::string>    symboltype_map;
	string                   string_buf;
	string                   shader;

	bool                     canNowAddFragment      = true;
	bool                     previewsCanAddFragment = false;
	string                   defered_fragment;

	size_t                   pos = 0, len = 0;

	std::map<string, string> macros;
	string                   current_define;

	CCryFile*                m_CurrentFile;
};
