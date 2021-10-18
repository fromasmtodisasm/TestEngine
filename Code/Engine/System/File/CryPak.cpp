#ifndef LINUX
#	include <BlackBox/Core/Platform/Platform.hpp>
#	include <BlackBox/Core/Platform/Windows.hpp>
#	include <BlackBox/System/File/CryPak.hpp>

#	include <algorithm>
#	include <cctype>
#	include <cstdarg>
#	include <string>

CCryPak::CCryPak(IMiniLog* pLog, PakVars* pPakVars, const bool bLvlRes)
	: m_pLog(pLog)
{
}

CCryPak::~CCryPak()
{
}

bool CCryPak::Init(const char* szBasePath)
{
	return false;
}

void CCryPak::Release()
{
	if (this != nullptr)
		delete this;
}

bool CCryPak::OpenPack(const char* pName, unsigned nFlags /* = FLAGS_PATH_REAL*/)
{
	int	 err;
	CryLog("$3Open Pack: %s", pName);
	auto a = zip_open(pName, ZIP_RDONLY, &err);
	if (!a) {
		auto e = zip_get_error(a);
		CryError("[ZIP] %s", e);
		return false;
	}
	m_Archives.insert({string(pName), a});
	return true;
}

bool CCryPak::OpenPack(const char* pBindingRoot, const char* pName, unsigned nFlags /* = FLAGS_PATH_REAL*/)
{
	return false;
}

bool CCryPak::ClosePack(const char* pName, unsigned nFlags /* = FLAGS_PATH_REAL*/)
{
	return false;
}

bool CCryPak::OpenPacks(const char* pWildcard, unsigned nFlags /* = FLAGS_PATH_REAL*/)
{

	return false;
}

bool CCryPak::OpenPacks(const char* pBindingRoot, const char* pWildcard, unsigned nFlags /* = FLAGS_PATH_REAL*/)
{
	return false;
}

bool CCryPak::ClosePacks(const char* pWildcard, unsigned nFlags /* = FLAGS_PATH_REAL*/)
{
	return false;
}

void CCryPak::AddMod(const char* szMod)
{
	std::string strPrepend = szMod;
	strPrepend.replace(strPrepend.begin(), strPrepend.end(), g_cNativeSlash, g_cNonNativeSlash);
	std::transform(strPrepend.begin(), strPrepend.end(), strPrepend.begin(),
				   [](unsigned char c)
				   { return std::tolower(c); });

	std::vector<std::string>::iterator strit;
	for (strit = m_arrMods.begin(); strit != m_arrMods.end(); ++strit)
	{
		std::string& sMOD = *strit;
		if (_stricmp(sMOD.c_str(), strPrepend.c_str()) == 0)
			return; // already added
	}
	m_arrMods.push_back(strPrepend);
}

void CCryPak::RemoveMod(const char* szMod)
{
	std::string strPrepend = szMod;
	strPrepend.replace(strPrepend.begin(), strPrepend.end(), g_cNativeSlash, g_cNonNativeSlash);
	std::transform(strPrepend.begin(), strPrepend.end(), strPrepend.begin(),
				   [](unsigned char c)
				   { return std::tolower(c); });

	std::vector<std::string>::iterator it;
	for (it = m_arrMods.begin(); it != m_arrMods.end(); ++it)
	{
		std::string& sMOD = *it;
		if (_stricmp(sMOD.c_str(), strPrepend.c_str()) == 0)
		{
			m_arrMods.erase(it);
			break;
		}
	} //it
}

ICryPak::PakInfo* CCryPak::GetPakInfo()
{
	return nullptr;
}

void CCryPak::FreePakInfo(PakInfo*)
{
}

FILE* CCryPak::FOpen(const char* pName, const char* mode, unsigned nFlags /* = 0*/)
{
	auto file = fopen( pName,mode );
	if (!file)
	{
		string adjustedName = m_DataRoot + pName;
		file = fopen(adjustedName.data(), mode);
	}
	return file;
}

FILE* CCryPak::FOpen(const char* pName, const char* mode, char* szFileGamePath, int nLen)
{
	return fopen((szFileGamePath + std::string(pName)).c_str(), mode);
}

size_t CCryPak::FRead(void* data, size_t length, size_t elems, FILE* handle)
{
	return fread(data, length, elems, handle);
}

size_t CCryPak::FWrite(void* data, size_t length, size_t elems, FILE* handle)
{
	return fwrite(data, length, elems, handle);
}

int CCryPak::FSeek(FILE* handle, long seek, int mode)
{
	return fseek(handle, seek, mode);
}

long CCryPak::FTell(FILE* handle)
{
	return ftell(handle);
}

int CCryPak::FClose(FILE* handle)
{
	if (handle)
		return fclose(handle);
	return EOF;
}

int CCryPak::FEof(FILE* handle)
{
	return feof(handle);
}

int CCryPak::FFlush(FILE* handle)
{
	return fflush(handle);
}

int CCryPak::FPrintf(FILE* handle, const char* format, ...)
{
	va_list ptr;
	va_start(ptr, format);
	auto res = vfprintf(handle, format, ptr);
	va_end(ptr);
	return res;
}

char* CCryPak::FGets(char* buffer, int max_count, FILE* file)
{
	return fgets(buffer, max_count, file);
}

int CCryPak::Getc(FILE* file)
{
	return getc(file);
}

int CCryPak::Ungetc(int c, FILE* file)
{
	return ungetc(c, file);
}

intptr_t CCryPak::FindFirst(const char* pDir, _finddata_t* fd)
{
	return _findfirst(pDir, fd);
}

int CCryPak::FindNext(intptr_t handle, _finddata_t* fd)
{
	return _findnext(handle, fd);
}

int CCryPak::FindClose(intptr_t handle)
{
	return _findclose(handle);
}

FILETIME CCryPak::GetModificationTime(FILE* f)
{
	return FILETIME();
}

bool CCryPak::MakeDir(const char* szPath)
{
	return _mkdir(szPath);
}

ICryArchive* CCryPak::OpenArchive(const char* szPath, unsigned nFlags /* = FLAGS_PATH_REAL*/)
{
	return nullptr;
}

const char* CCryPak::GetFileArchivePath(FILE* f)
{
	return nullptr;
}

int CCryPak::RawCompress(const void* pUncompressed, unsigned long* pDestSize, void* pCompressed, unsigned long nSrcSize, int nLevel /* = -1*/)
{
	return 0;
}

int CCryPak::RawUncompress(void* pUncompressed, unsigned long* pDestSize, const void* pCompressed, unsigned long nSrcSize)
{
	return 0;
}

void CCryPak::RecordFileOpen(bool bEnable)
{
}

void CCryPak::RecordFile(const char* szFilename)
{
}

void CCryPak::EnumerateRecordedFiles(RecordedFilesEnumCallback enumCallback)
{
}

void CCryPak::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
}
FILE* CCryPak::FOpenRaw(const char* pName, const char* mode)
{
	return nullptr;
}
size_t CCryPak::FReadRaw(void* data, size_t length, size_t elems, FILE* handle)
{
	return FRead(data, length, elems, handle);
}

const char* CCryPak::AdjustFileName(const char* szSourcePath, char szDestPath[g_nMaxPath], unsigned nFlags)
{
	strncpy(szDestPath, szSourcePath, g_nMaxPath);
	return szSourcePath;
}
#endif