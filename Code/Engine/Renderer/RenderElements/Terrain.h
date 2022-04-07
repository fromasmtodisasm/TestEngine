#include <BlackBox/Renderer/RendElement.h>

class Terrain : public CRendElement
{
public:
	Terrain()
	{
		m_Type = EDataType::eDATA_Terrain;
		m_Flags = 0;
	}
	EDataType                mfGetType() { return m_Type; }

	void                     mfSetType(EDataType t) { m_Type = t; }

	uint                     mfGetFlags(void) { return m_Flags; }
	void                     mfSetFlags(uint fl) { m_Flags = fl; }
	void                     mfUpdateFlags(uint fl) { m_Flags |= fl; }
	void                     mfClearFlags(uint fl) { m_Flags &= ~fl; }

	virtual void             mfPrepare() override;
	virtual bool             mfCullByClipPlane(CCObject* pObj) override;
	virtual CMatInfo*        mfGetMatInfo() override;
	virtual list2<CMatInfo>* mfGetMatInfoList() override;
	virtual int              mfGetMatId() override;
	virtual bool             mfCull(CCObject* obj) override;
	virtual bool             mfCull(CCObject* obj, SShader* ef) override;
	virtual void             mfReset() override;
	virtual CRendElement*    mfCopyConstruct(void) override;
	virtual void             mfCenter(Vec3& centr, CCObject* pObj) override;
	virtual void             mfGetPlane(Plane& pl) override;
	virtual float            mfDistanceToCameraSquared(const CCObject& thisObject) override;
	virtual void             mfEndFlush() override;
	virtual int              mfTransform(Matrix44& ViewMatr, Matrix44& ProjMatr, vec4_t* verts, vec4_t* vertsp, int Num) override;
	virtual bool             mfIsValidTime(SShader* ef, CCObject* obj, float curtime) override;
	virtual void             mfBuildGeometry(SShader* ef) override;
	virtual bool             mfCompile(SShader* ef, char* scr) override;
	virtual CRendElement*    mfCreateWorldRE(SShader* ef, SInpData* ds) override;
	virtual bool             mfDraw(SShader* ef, SShaderPass* sfm) override;
	virtual void*            mfGetPointer(ESrcPointer ePT, int* Stride, int Type, ESrcPointer Dst, int Flags) override;
	virtual bool             mfPreDraw(SShaderPass* sl)
	{
		assert(0);
		return false;
	}
	virtual bool             mfCheckUpdate(int nVertFormat, int Flags)
	{
		int i = Flags;
		return true;
	}
	virtual int Size() { return 0; }
};