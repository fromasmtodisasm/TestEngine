#include "StreamReadStream.h"

bool CReadStream::IsError()
{
	return false;
}

bool CReadStream::IsFinished()
{
	return false;
}

unsigned int CReadStream::GetBytesRead(bool bWait/* = false*/)
{
	return 0;
}

const void* CReadStream::GetBuffer()
{
	return nullptr;
}

DWORD_PTR CReadStream::GetUserData()
{
	return DWORD_PTR();
}

void CReadStream::Wait()
{
}
