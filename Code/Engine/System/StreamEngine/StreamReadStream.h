#pragma once

#include <BlackBox/System/IStreamEngine.h>

class CReadStream : public IReadStream
{
public:
	CReadStream();
	virtual ~CReadStream();

	// Inherited via IReadStream
	virtual bool IsError() override;
	virtual bool IsFinished() override;
	virtual unsigned int GetBytesRead(bool bWait = false) override;
	virtual const void*  GetBuffer() override;
	virtual DWORD_PTR    GetUserData() override;
	virtual void         Wait() override;
};