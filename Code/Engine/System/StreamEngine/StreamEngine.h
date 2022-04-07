#pragma once

#include <BlackBox/System/IStreamEngine.h>

//////////////////////////////////////////////////////////////////////////
class CStreamEngine : public IStreamEngine, public ISystemEventListener, public IInputEventListener
{
public:
	CStreamEngine();
	~CStreamEngine();

	void Shutdown();
	// This is called to cancel all pending requests, without sending callbacks.
	void CancelAll();

	//////////////////////////////////////////////////////////////////////////
	// IStreamEngine interface
	//////////////////////////////////////////////////////////////////////////
	virtual IReadStreamPtr StartRead(const char* szSource, const char* szFile, IStreamCallback* pCallback = NULL, StreamReadParams* pParams = NULL) override;
	virtual unsigned       GetFileSize(const char* szFile, unsigned nCryPakFlags = 0) override;
	virtual void           Update(unsigned nFlags = 0) override;
	virtual unsigned       Wait(unsigned nMilliseconds, unsigned nFlags = 0) override;
	virtual void           GetMemoryStatistics(ICrySizer* pSizer) override;
	virtual DWORD          GetStreamCompressionMask() const override;

	// Inherited via ISystemEventListener
	virtual void           OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;

	// Inherited via IInputEventListener
	virtual bool           OnInputEvent(const SInputEvent& event) override;
};