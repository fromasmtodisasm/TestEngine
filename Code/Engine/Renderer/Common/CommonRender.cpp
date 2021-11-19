#include "CommonRender.h"
// NameTable internal variable.
CNameTableR* CCryNameR::ms_table;

void CBaseResource::UnregisterAndDelete()
{
}

bool CBaseResource::IsValid() const
{
	return false;
}

CBaseResource* CBaseResource::GetResource(const CCryNameTSCRC& className, int nID, bool bAddRef)
{
	return nullptr;
}

CBaseResource* CBaseResource::GetResource(const CCryNameTSCRC& className, const CCryNameTSCRC& Name, bool bAddRef)
{
	return nullptr;
}

SResourceContainer* CBaseResource::GetResourcesForClass(const CCryNameTSCRC& className)
{
	return nullptr;
}

void CBaseResource::ShutDown()
{
}

bool CBaseResource::Register(const CCryNameTSCRC& resName, const CCryNameTSCRC& Name)
{
	return false;
}

bool CBaseResource::UnRegister()
{
	return false;
}
