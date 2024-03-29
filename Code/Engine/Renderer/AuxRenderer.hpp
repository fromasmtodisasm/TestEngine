#pragma once
#include <BlackBox/Renderer/IRender.hpp>
#include <BlackBox/Renderer/IRenderAuxGeom.hpp>

#if 0
using BB_VERTEX	 = SVF_P3F_C4B;
#else
// using BB_VERTEX	 = SVF_P3F_C4B_T2F;
using BB_VERTEX = SVF_P3F_N_T2F;

#endif

class CShader;
struct SAuxPushBufferEntry
{
	SAuxPushBufferEntry() = default;
	SAuxPushBufferEntry(uint32 nv, RenderPrimitive rpt)
		: m_numVertices(nv)
		, m_primitive(rpt)
	{
	}
	uint32			m_numVertices;
	RenderPrimitive m_primitive;
};

struct alignas(16) SAABBConstantBuffer
{
	Legacy::Mat4 Model;
	Legacy::Vec3 LightPos;
};

#if 0
using SAABBBuffer = gl::ConstantBuffer<SAABBConstantBuffer>;
using SAABBBufferPtr = std::shared_ptr<SAABBBuffer>;
#endif

struct Material
{
	UCol DiffuseColor{Legacy::Vec3{0, 1, 1}};
	UCol SpecularColor{Legacy::Vec3{1,1,1}};
	UCol Gloss;
};

struct SDrawElement
{
	CVertexBuffer*      m_pBuffer{};
	SVertexStream*      m_Inices{};
	glm::mat4           m_Transform{1};
	int					m_DiffuseMap;
	Material            m_Material;
	_smart_ptr<CShader> m_Shader;
};

using AuxPushBuffer	  = std::vector<SAuxPushBufferEntry>;
using AuxVertexBuffer = std::vector<SAuxVertex>;
using AuxIndexBuffer  = std::vector<vtx_idx>;

class CRenderAuxGeom : public IRenderAuxGeom
{
	const int INIT_VB_SIZE = 1024 * 4;
	using BoundingBox	   = std::array<BB_VERTEX, 36>;

  public:
	CRenderAuxGeom();
	~CRenderAuxGeom();
	void DrawAABB(Legacy::Vec3 min, Legacy::Vec3 max, const UCol& col) override;
	void DrawTriangle(const Legacy::Vec3& v0, const UCol& colV0, const Legacy::Vec3& v1, const UCol& colV1, const Legacy::Vec3& v2, const UCol& colV2) override;
	void DrawLine(const Legacy::Vec3& v0, const UCol& colV0, const Legacy::Vec3& v1, const UCol& colV1, float thickness = 1.0f) override;
	void DrawLines(const Legacy::Vec3* v, uint32 numPoints, const UCol& col, float thickness = 1.0f) override;
	void PushImage(const SRender2DImageDescription& image) override;
	void Flush() override;
	void DrawMesh(CVertexBuffer* pVertexBuffer, SVertexStream* pIndices, glm::mat4 transform, int texture) override;

  private:
	void AddPrimitive(SAuxVertex*& pVertices, uint32 numVertices, RenderPrimitive primitive);
	void DrawAABBs();
	void DrawLines();
	HRESULT InitCube();
	void	DrawElement(const SDrawElement& DrawElement);
	void	DrawElementToZBuffer(const SDrawElement& DrawElement);


  private:
	CVertexBuffer*		m_BoundingBox;
	AuxPushBuffer		m_auxPushBuffer;
	AuxVertexBuffer		m_VB;
	CVertexBuffer*		m_HardwareVB;
	_smart_ptr<IShader> m_BoundingBoxShader;
	_smart_ptr<IShader> m_AuxGeomShader;

	std::vector<SRender2DImageDescription> m_Images;
	std::vector<SDrawElement> m_Meshes;
	std::vector<BoundingBox> m_BBVerts;

	int m_CurrentVB_Size = INIT_VB_SIZE;
	int dbg_mode		 = 0;
	int stop			 = 0;

	CShader* m_IllumShader;
	CShader* m_ZPShader;

  public:
	static ID3D11DepthStencilState* m_pDSStateZPrePass;
	static ID3D11DepthStencilState* m_pDSStateMesh;
	static ID3D11DepthStencilState* m_pDSStateLines;
};
