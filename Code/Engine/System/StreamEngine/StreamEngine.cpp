#include "StreamEngine.h"

CStreamEngine::CStreamEngine()
{
}

CStreamEngine::~CStreamEngine()
{
}

void CStreamEngine::Shutdown()
{
}

void CStreamEngine::CancelAll()
{
}

IReadStreamPtr CStreamEngine::StartRead(const char* szSource, const char* szFile, IStreamCallback* pCallback/* = NULL*/, StreamReadParams* pParams/* = NULL*/)
{
	return IReadStreamPtr();
}

unsigned CStreamEngine::GetFileSize(const char* szFile, unsigned nCryPakFlags/* = 0*/)
{
	return 0;
}

void CStreamEngine::Update(unsigned nFlags/* = 0*/)
{
}

unsigned CStreamEngine::Wait(unsigned nMilliseconds, unsigned nFlags/* = 0*/)
{
	return 0;
}

void CStreamEngine::GetMemoryStatistics(ICrySizer* pSizer)
{
}

DWORD CStreamEngine::GetStreamCompressionMask() const
{
	return 0;
}

void CStreamEngine::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
}

bool CStreamEngine::OnInputEvent(const SInputEvent& event)
{
	return false;
}
