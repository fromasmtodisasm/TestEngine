
//////////////////////////////////////////////////////////////////////
//
//	Crytek Source code
//	Copyright (c) Crytek 2001-2004
//
//	File: IBitStream.h
//	Description:
//		Interface for bit stream class.
//
//	History:
//	-	September 2002: File created
//	- February 2005: Modified by Marco Corbetta for SDK release
//
//////////////////////////////////////////////////////////////////////

#ifndef IBITSTREAM_H__
#define IBITSTREAM_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

class CStream;

#include <BlackBox/Core/Platform/Platform.hpp>

//////////////////////////////////////////////////////////////////////
enum eBitStreamHint
{
	eDoNotCompress,     // ...
	e8BitNormal,        // Legacy::Vec3,														low quality normalized vector
	eWorldPos,          // Legacy::Vec3,														absolute world position
	eASCIIText,         // char *,													static huffman compression
	eEntityId,          // u __int32,__int32, u __int16			16bit, some entities have higher probability (e.g. player)
	eEntityClassId,     // __int32,u __int16,								for entity creation
	e8BitRGB,           // Legacy::Vec3,														8bit Color
	e4BitRGB,           // Legacy::Vec3,														4bit Color
	eQuaternion,        // Legacy::Vec3,														eQuaternion
	eEulerAnglesHQ,     // Legacy::Vec3,														YAW,PITCH,ROLL cyclic in [0..360[, special compression if PITCH=0 (not float but still quite high quality)
	eSignedUnitValueLQ, // float,														[-1..1] 8+1+1 bit if not zero, 1 bit of zero
};

//////////////////////////////////////////////////////////////////////
struct IBitStream
{
	//!
	//  virtual bool ReadBitStream( bool &Value )=0;
	//!
	virtual bool ReadBitStream(CStream& stm, int8& Value, const eBitStreamHint eHint)                                 = 0;
	//!
	virtual bool ReadBitStream(CStream& stm, int16& Value, const eBitStreamHint eHint)                                = 0;
	//!
	virtual bool ReadBitStream(CStream& stm, int32& Value, const eBitStreamHint eHint)                                = 0;
	//!
	virtual bool ReadBitStream(CStream& stm, uint8& Value, const eBitStreamHint eHint)                                = 0;
	//!
	virtual bool ReadBitStream(CStream& stm, uint16& Value, const eBitStreamHint eHint)                               = 0;
	//!
	virtual bool ReadBitStream(CStream& stm, uint32& Value, const eBitStreamHint eHint)                               = 0;
	//!
	virtual bool ReadBitStream(CStream& stm, float& Value, const eBitStreamHint eHint)                                = 0;
	//!
	virtual bool ReadBitStream(CStream& stm, Legacy::Vec3& Value, const eBitStreamHint eHint)                         = 0;
	//!
	//! max 256 characters
	virtual bool ReadBitStream(CStream& stm, char* Value, const DWORD nBufferSize, const eBitStreamHint eHint)        = 0;

	//!
	virtual bool WriteBitStream(CStream& stm, const int8 Value, const eBitStreamHint eHint)                           = 0;
	//!
	virtual bool WriteBitStream(CStream& stm, const int16 Value, const eBitStreamHint eHint)                          = 0;
	//!
	virtual bool WriteBitStream(CStream& stm, const int32 Value, const eBitStreamHint eHint)                          = 0;
	//!
	virtual bool WriteBitStream(CStream& stm, const uint8 Value, const eBitStreamHint eHint)                          = 0;
	//!
	virtual bool WriteBitStream(CStream& stm, const uint16 Value, const eBitStreamHint eHint)                         = 0;
	//!
	virtual bool WriteBitStream(CStream& stm, const uint32 Value, const eBitStreamHint eHint)                         = 0;
	//!
	virtual bool WriteBitStream(CStream& stm, const float Value, const eBitStreamHint eHint)                          = 0;
	//!
	virtual bool WriteBitStream(CStream& stm, const Legacy::Vec3& Value, const eBitStreamHint eHint)                  = 0;
	//!
	//! max 256 characters
	virtual bool WriteBitStream(CStream& stm, const char* Value, const DWORD nBufferSize, const eBitStreamHint eHint) = 0;

	// the following methods make use of the WriteBitStream and the ReadBitStream to simulate the error the
	// write and read operations would have

	//! to get the compression error
	virtual void SimulateWriteRead(int8& Value, const eBitStreamHint eHint)                                           = 0;
	//! to get the compression error
	virtual void SimulateWriteRead(int16& Value, const eBitStreamHint eHint)                                          = 0;
	//! to get the compression error
	virtual void SimulateWriteRead(int32& Value, const eBitStreamHint eHint)                                          = 0;
	//! to get the compression error
	virtual void SimulateWriteRead(uint8& Value, const eBitStreamHint eHint)                                          = 0;
	//! to get the compression error
	virtual void SimulateWriteRead(uint16& Value, const eBitStreamHint eHint)                                         = 0;
	//! to get the compression error
	virtual void SimulateWriteRead(uint32& Value, const eBitStreamHint eHint)                                         = 0;
	//! to get the compression error
	virtual void SimulateWriteRead(float& Value, const eBitStreamHint eHint)                                          = 0;
	//! to get the compression error
	virtual void SimulateWriteRead(Legacy::Vec3& Value, const eBitStreamHint eHint)                                   = 0;
	//! to get the compression error
	virtual void SimulateWriteRead(char* Value, const DWORD nBufferSize, const eBitStreamHint eHint)                  = 0;
};

#endif //IBITSTREAM_H__
