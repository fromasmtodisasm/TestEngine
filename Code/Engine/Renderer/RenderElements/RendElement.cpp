#include <BlackBox/Renderer/RendElement.h>
//#include <Cry_Geo.h>

using CRenderElement = CRendElement;

CRenderElement CRenderElement::s_RootGlobal(true);

CRendElement::CRendElement()
{
	m_Type       = eDATA_Unknown;
	m_NextGlobal = NULL;
	m_PrevGlobal = NULL;
	m_Flags      = 0;
	m_CustomData = NULL;
	for (int i = 0; i < MAX_CUSTOM_TEX_BINDS_NUM; i++)
		m_CustomTexBind[i] = -1;
	m_fFogScale = 0;
	m_SortId    = 0;
	m_LastVP    = NULL;
	if (!s_RootGlobal.m_NextGlobal)
	{
		s_RootGlobal.m_NextGlobal = &s_RootGlobal;
		s_RootGlobal.m_PrevGlobal = &s_RootGlobal;
	}
	if (this != &s_RootGlobal)
		LinkGlobal(&s_RootGlobal);
}

CRendElement::CRendElement(bool bGlobal)
{
	m_Type = eDATA_Unknown;
	if (!s_RootGlobal.m_NextGlobal)
	{
		s_RootGlobal.m_NextGlobal = &s_RootGlobal;
		s_RootGlobal.m_PrevGlobal = &s_RootGlobal;
#if 0
		for (int i = 0; i < 4; i++)
		{
			s_pRootRelease[i] = new CRenderElement(true);
			s_pRootRelease[i]->m_NextGlobal = s_pRootRelease[i];
			s_pRootRelease[i]->m_PrevGlobal = s_pRootRelease[i];
		}
#endif
	}

#if 0
	m_Flags = FCEF_NONE;
	m_nFrameUpdated = -1;
	m_CustomData = NULL;
	m_nID = CRenderElement::s_nCounter++;
#endif
	int i;
	for (i = 0; i < /*CRenderElement::*/ MAX_CUSTOM_TEX_BINDS_NUM; i++)
		m_CustomTexBind[i] = -1;
}

const char* CRendElement::mfTypeString()
{
	switch (m_Type)
	{
	case eDATA_Unknown:
		return "Unknown";
	case eDATA_Dummy:
		return "Dummy";
	case eDATA_Sky:
		return "Sky";
	case eDATA_Beam:
		return "Beam";
	case eDATA_Poly:
		return "Poly";
	case eDATA_Curve:
		return "Curve";
	case eDATA_MotModel:
		return "MotModel";
	case eDATA_MeshModel:
		return "MeshModel";
	case eDATA_PolyBlend:
		return "PolyBlend";
	case eDATA_AnimPolyBlend:
		return "AnimPolyBlend";
	case eDATA_ClientPoly:
		return "ClientPoly";
	case eDATA_ClientPoly2D:
		return "ClientPoly2D";
	case eDATA_ParticleSpray:
		return "ParticleSpray";
	case eDATA_TriMesh:
		return "TriMesh";
	case eDATA_TriMeshShadow:
		return "TriMeshShadow";
	case eDATA_Prefab:
		return "Prefab";
	case eDATA_Flare:
		return "Flare";
	case eDATA_FlareGeom:
		return "FlareGeom";
	case eDATA_FlareProp:
		return "FlareProp";
	case eDATA_Tree:
		return "Tree";
	case eDATA_Tree_Leaves:
		return "Tree_Leaves";
	case eDATA_Tree_Branches:
		return "Tree_Branches";
	case eDATA_Terrain:
		return "Terrain";
	case eDATA_SkyZone:
		return "SkyZone";
	case eDATA_OcLeaf:
		return "OcLeaf";
	case eDATA_TerrainSector:
		return "TerrainSector";
	case eDATA_2DQuad:
		return "2DQuad";
	case eDATA_FarTreeSprites:
		return "FarTreeSprites";
	case eDATA_AnimModel:
		return "AnimModel";
	case eDATA_MotionBlur:
		return "MotionBlur";
	case eDATA_ShadowMapGen:
		return "ShadowMapGen";
	case eDATA_TerrainDetailTextureLayers:
		return "TerrainDetailTextureLayers";
	case eDATA_TerrainParticles:
		return "TerrainParticles";
	case eDATA_Ocean:
		return "Ocean";
	case eDATA_Glare:
		return "Glare";
	case eDATA_OcclusionQuery:
		return "OcclusionQuery";
	case eDATA_TempMesh:
		return "TempMesh";
	case eDATA_ClearStencil:
		return "ClearStencil";
	case eDATA_FlashBang:
		return "FlashBang";
	case eDATA_ScreenProcess:
		return "ScreenProcess";
	case eDATA_HDRProcess:
		return "HDRProcess";
	default:
		assert(0);
		return "";
	}
}

void             CRendElement::mfPrepare() {}
bool             CRendElement::mfCullByClipPlane(CCObject* pObj) { return false; }
CMatInfo*        CRendElement::mfGetMatInfo() { return nullptr; }
list2<CMatInfo>* CRendElement::mfGetMatInfoList() { return nullptr; }
int              CRendElement::mfGetMatId() { return 0; }
bool             CRendElement::mfCull(CCObject* obj) { return false; }
bool             CRendElement::mfCull(CCObject* obj, SShader* ef) { return false; }
void             CRendElement::mfReset() {}
CRendElement*    CRendElement::mfCopyConstruct(void) { return nullptr; }
void             CRendElement::mfCenter(Vec3& Pos, CCObject* pObj)
{
	//FIXME:
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
void  CRendElement::mfGetPlane(Plane& pl) {}
float CRendElement::mfDistanceToCameraSquared(const CCObject& thisObject)
{
	assert(0);
	return 0;
}
void CRendElement::mfEndFlush() {}
void CRendElement::Release()
{
#if 0
	m_Flags |= FCEF_DELETED;
#endif

#ifdef _DEBUG
	//if (gRenDev && gRenDev->m_pRT)
	//  assert(gRenDev->m_pRT->IsMainThread(true));
#endif
	m_Type = eDATA_Unknown;
#if 0
	if (bForce)
	{
		//sDeleteRE(this);
		delete this;
		return;
	}
#endif
	int nFrame = Env::Renderer()->GetFrameID();

#if 0
	AUTO_LOCK(s_accessLock);
#endif
	CRenderElement& Root = CRenderElement::s_RootGlobal;
	UnlinkGlobal();
	LinkGlobal(&Root);
	//sDeleteRE(this);
}
int CRendElement::mfTransform(Matrix44& ViewMatr, Matrix44& ProjMatr, vec4_t* verts, vec4_t* vertsp, int Num)
{
	assert(0);
	return 0;
}
bool CRendElement::mfIsValidTime(SShader* ef, CCObject* obj, float curtime)
{
	assert(0);
	return false;
}
void CRendElement::mfBuildGeometry(SShader* ef) {}
bool CRendElement::mfCompile(SShader* ef, char* scr)
{
	assert(0);
	return false;
}
CRendElement* CRendElement::mfCreateWorldRE(SShader* ef, SInpData* ds)
{
	assert(0);
	return 0;
}
bool CRendElement::mfDraw(SShader* ef, SShaderPass* sfm)
{
	assert(0);
	return false;
}
void* CRendElement::mfGetPointer(ESrcPointer ePT, int* Stride, int Type, ESrcPointer Dst, int Flags)
{
	assert(0);
	return nullptr;
}
