//FIXME:
//#include <Cry_Geo.h>

#include "Terrain.h"

void             Terrain::mfPrepare() {}
bool             Terrain::mfCullByClipPlane(CCObject* pObj) { return false; }
CMatInfo*        Terrain::mfGetMatInfo() { return nullptr; }
list2<CMatInfo>* Terrain::mfGetMatInfoList() { return nullptr; }
int              Terrain::mfGetMatId() { return 0; }
bool             Terrain::mfCull(CCObject* obj) { return false; }
bool             Terrain::mfCull(CCObject* obj, SShader* ef) { return false; }
void             Terrain::mfReset() {}
CRendElement*    Terrain::mfCopyConstruct(void) { return nullptr; }
void             Terrain::mfCenter(Vec3& Pos, CCObject* pObj)
{
	#if 0
	AABB bb;
	mfGetBBox(bb.min, bb.max);

	Pos = 0.5f * (bb.min + bb.max);
	if (pObj)
		Pos += pObj->GetTranslation();
	#else
	assert(0);
	#endif
}
void  Terrain::mfGetPlane(Plane& pl) {}
float Terrain::mfDistanceToCameraSquared(const CCObject& thisObject)
{
	assert(0);
	return 0;
}
void Terrain::mfEndFlush() {}
void Release() {}
int  Terrain::mfTransform(Matrix44& ViewMatr, Matrix44& ProjMatr, vec4_t* verts, vec4_t* vertsp, int Num)
{
	assert(0);
	return 0;
}
bool Terrain::mfIsValidTime(SShader* ef, CCObject* obj, float curtime)
{
	assert(0);
	return false;
}
void Terrain::mfBuildGeometry(SShader* ef) {}
bool Terrain::mfCompile(SShader* ef, char* scr)
{
	assert(0);
	return false;
}
CRendElement* Terrain::mfCreateWorldRE(SShader* ef, SInpData* ds)
{
	assert(0);
	return 0;
}
bool Terrain::mfDraw(SShader* ef, SShaderPass* sfm)
{
	assert(0);
	return false;
}
void* Terrain::mfGetPointer(ESrcPointer ePT, int* Stride, int Type, ESrcPointer Dst, int Flags)
{
	assert(0);
	return nullptr;
}