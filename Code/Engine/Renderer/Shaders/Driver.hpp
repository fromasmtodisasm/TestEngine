#pragma once

#include <BlackBox/System/File/CryFile.h>

#include <map>
#include <string>
#include <fstream>
#define YYDEBUG                        1
#define YYFPRINTF(stderr, format, ...) CryError(format, __VA_ARGS__)
#pragma warning(push, 0)
#include "Parser.hpp"
#pragma warning(pop)
//#include "Scanner.hpp"

#ifdef PARSERDRIVER_EXPORTS
	#define PARSERDRIVER_API DLL_EXPORT
#else
	#define PARSERDRIVER_API DLL_IMPORT
#endif

class CEffect;
struct IEffect;

// https://habr.com/ru/post/326578/#Example_4
using std::streampos;
class CryPakBuffer : public std::streambuf
{
private:
	std::vector<char_type> buffer;
	CCryFile               file;
	streampos              pos_base; //позиция в файле для eback

	streampos              fill_buffer_from(streampos newpos, int dir = SEEK_SET)
	{
		//if (!file || fseek(file, newpos, dir) == -1)
		if (!file || file.Seek(newpos, dir) == -1)
			return -1;
		//запоминаем текущую позицию в файле для eback
		long pos = file.GetPosition();
		if (pos < 0)
			return -1;
		pos_base         = pos;

		char_type* start = eback();
		//читаем не больше символов, чем вмещает буфер
		//size_t     rd    = fread(start, sizeof(char_type), buffer.size(), file);
		size_t     rd    = file.ReadRaw(start, buffer.size());
		//указываем размер буфера не больше, чем было считано символов
		setg(start, start, start + rd);

		return rd > 0 && pos_base >= 0 ? pos_base : streampos(-1);
	}

protected:
	virtual int underflow() override
	{
		if (!file)
			return traits_type::eof();

		if (gptr() < egptr()) //если буфер не пуст, вернем текущий символ
			return *gptr();

		streampos pos;
		if (pos_base < 0)
		{ //если буфер еще не заполнялся, заполняем с начала
			pos = fill_buffer_from(0);
		}
		else
		{ //иначе заполняем со следующего несчитанного символа
			pos = fill_buffer_from(pos_base + egptr() - eback());
		}

		return pos != streampos(-1) ? *gptr() : traits_type::eof();
	}

	//второй аргумент в нашем случае всегда содержит ios_base::in
	//однако в общем случае может содержать и ios_base::out и даже сразу оба (побитовое ИЛИ)
	virtual streampos seekpos(streampos sp, std::ios_base::openmode which) override
	{
		if (!(which & std::ios_base::in))
			return streampos(-1);
		return fill_buffer_from(sp);
	}

	//обработка трех вариантов позиционирования: с начала, с текущей позиции и с конца
	virtual streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which) override
	{
		using std::ios_base;

		if (!(which & std::ios_base::in))
			return streampos(-1);

		switch (way)
		{
		default:
		case ios_base::beg:
			return fill_buffer_from(off, SEEK_SET);
		case ios_base::cur:
			return fill_buffer_from(pos_base + gptr() - eback() + off); //учитываем позицию от начала в нашем буфере
		case ios_base::end:
			return fill_buffer_from(off, SEEK_END);
		}
	}

	virtual int pbackfail(int c) override
	{
		//когда gptr > eback, значит в буфере есть символ на нужной позиции,
		//но он не совпал с переданным, запрещаем
		if (pos_base <= 0 || gptr() > eback())
			return traits_type::eof();

		//загружаем в буфер данные, начиная с предыдущего символа
		if (fill_buffer_from(pos_base - streampos(1L)) == streampos(-1))
			return traits_type::eof();

		if (*gptr() != c)
		{
			gbump(1); //возвращаемся назад, неудачная операция
			return traits_type::eof();
		}

		return *gptr();
	}

public:
	CryPakBuffer(size_t _bufsize)
	    : buffer(_bufsize)
	    , pos_base(-1)
	{
		char_type* start = buffer.data();
		char_type* end   = start + buffer.size();
		setg(start, end, end); //устанавливаем eback = start, gptr = end, egptr = end
	}

	~CryPakBuffer()
	{
		close();
	}

	bool open(string fn)
	{
		close();
		return file.Open(fn.c_str(), "rb");
	}

	void close()
	{
		if (file)
		{
			file.Close();
		}
	}
    [[nodiscard]] 
	bool is_open() const {
        return static_cast<bool>(file);
    }
};

struct CryPakIStream : std::istream
{
	const uint   bufferSize = 512;
	CryPakBuffer buf{bufferSize};
	CryPakIStream()
	    : std::istream(&buf)
	{
	}
	bool open(std::string_view name)
	{
		return buf.open(name.data());
	}
	void close()
	{
		buf.close();
	}

	[[nodiscard]]
	bool is_open()
	{
		return buf.is_open();
	}
};

struct IDriver
{
	virtual ~IDriver()
	{
	}
	//virtual bool parse(const std::string& f) = 0;
	virtual IEffect* parse(const char* f) = 0;
	virtual void     Release()            = 0;
};

class Driver : public IDriver
{
public:
	Driver();
	~Driver();
	//bool parse(const std::string& f) override;
	IEffect*     parse(const char* f) override;

	bool         LoadEffectFromFile(IEffect* pEffect, const char* filename);
	bool         LoadEffect(IEffect* pEffect, const char* str);

	void         ScanBegin(const char* file);
	void         ScanEnd();

	bool         trace_scanning;
	yy::location location;
	std::string  file;
	bool         trace_parsing;

	friend class Scanner;

	Scanner*                      scanner;
	yy::parser                    parser;
	CEffect*                      currentEffect;
	std::vector<std::string_view> CommonCode;

private:
	CryPakIStream stream;
	//CryPakBuffer pakBuffer;

	// Óíàñëåäîâàíî ÷åðåç IDriver
	virtual void Release() override;
};

typedef IDriver* (*PFNCREATEDRIVERINTERFACE)();
extern "C"
{
	PARSERDRIVER_API IDriver* CreateParserDriver();
}
