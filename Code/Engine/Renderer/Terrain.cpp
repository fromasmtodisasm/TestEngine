#include <BlackBox/Renderer/Camera.hpp>
#include "D3D/Renderer.h"
#include "Terrain.h"

#include <BlackBox/Renderer/IRender.hpp>
#include <BlackBox/3DEngine/IStatObj.hpp>
#include <BlackBox/Renderer/IRenderAuxGeom.hpp>

//#include "StatObject.hpp"
#include "CryHeaders.h"
#include <BlackBox/System/ConsoleRegistration.h>

template<typename T>
struct ConstBuffer : public T
{
	using This = T;
	ComPtr<ID3DBuffer> Buffer;

	void               Write()
	{
		::GetDeviceContext()->UpdateSubresource(Buffer.Get(), 0, nullptr, (T*)this, sizeof(T), 0);
	}
};

template<typename T>
ConstBuffer<T> CreateCBuffer()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	// Create the constant buffer
	bd.Usage          = D3D11_USAGE_DEFAULT;
	bd.ByteWidth      = Memory::AlignedSizeCB<T>::value;
	bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	ConstBuffer<T> Data;
	auto           hr = GetDevice()->CreateBuffer(&bd, NULL, Data.Buffer.GetAddressOf());
	if (FAILED(hr))
	{
		CryFatalError("cannot create CBuffer");
		//return hr;
	}
	return Data;
}
namespace
{
	_smart_ptr<ID3D11RasterizerState> g_pRasterizerStateSolid{};
	_smart_ptr<ID3D11RasterizerState> g_pRasterizerStateWire{};
	_smart_ptr<ID3D11RasterizerState> g_pRasterizerStateForMeshCurrent{};
} // namespace

CTerrainRenderer::CTerrainRenderer()
{
	GenerateMesh(PatchSize);

	m_Shader = Env::Renderer()->Sh_Load("terrain");

	LoadTerrain("GrandCanyon");

	m_pRendElement = Env::Renderer()->EF_CreateRE(EDataType::eDATA_Terrain);
	// Set up rasterizer
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroStruct(rasterizerDesc);
	rasterizerDesc.CullMode              = D3D11_CULL_BACK;
	rasterizerDesc.FillMode              = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthBias             = false;
	rasterizerDesc.DepthBiasClamp        = 0;
	rasterizerDesc.SlopeScaledDepthBias  = 0;
	rasterizerDesc.DepthClipEnable       = true;
	rasterizerDesc.ScissorEnable         = false;
	rasterizerDesc.MultisampleEnable     = true;
	rasterizerDesc.AntialiasedLineEnable = true;

	GetDevice()->CreateRasterizerState(&rasterizerDesc, g_pRasterizerStateSolid.GetAddressOf());
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	GetDevice()->CreateRasterizerState(&rasterizerDesc, g_pRasterizerStateWire.GetAddressOf());

	g_pRasterizerStateForMeshCurrent = g_pRasterizerStateSolid;

	Env::Console()->AddConsoleVarSink(this);

	REGISTER_CVAR2("r_TerrainPatchSize", &CV_TerrainPatchSize, 64, 0, "");
	REGISTER_CVAR2("r_DrawDistance", &CV_DrawDistance, 500.f, 0, "Terrain patch draw distance");
	REGISTER_CVAR2("r_TerrainPatchScale", &SV_Scale, 100.f, 0, "Terrain patch scale");

	REGISTER_COMMAND("r_TerrainShaderReload", [](IConsoleCmdArgs*)
	    { gTerrainRenderer->m_Shader->Reload(0); },
	    0, "Reload terrain shader");
}

CTerrainRenderer::~CTerrainRenderer()
{
	//Env::Renderer()->ReleaseBuffer(m_pVerts);

	Env::Console()->RemoveConsoleVarSink(this);
	SAFE_DELETE(m_pRendElement);
}

void CTerrainRenderer::Render(CCamera& Camera)
{
	DrawAxises();

	PrepareForDrawing();

	RenderNodes(Camera);
}

void CTerrainRenderer::PrepareForDrawing()
{
	//::GetDeviceContext()->PSSetSamplers(0, 1, CGlobalResources::Get().LinearSampler.GetAddressOf());
	//::GetDeviceContext()->OMSetDepthStencilState(CRenderAuxGeom::m_pDSStateZPrePass, 0);
	//::GetDeviceContext()->RSSetState(g_pRasterizerStateWire);
	::GetDeviceContext()->RSSetState(g_pRasterizerStateSolid);
	//::GetDeviceContext()->OMSetBlendState(m_pBlendState, 0, 0xffffffff);

	//::GetDeviceContext()->RSSetState(g_pRasterizerStateForMeshCurrent);
	//m_pDSStateMeshCurrent = CRenderAuxGeom::m_pDSStateZPrePass;
	//::GetDeviceContext()->OMSetDepthStencilState(m_pDSStateMeshCurrent, 0);
}

void CTerrainRenderer::RenderNodes(CCamera& Camera)
{
	// Update our time
	static DWORD dwTimeStart    = 0;
	DWORD        dwTimeCur      = GetTickCount();

	auto         View           = Camera.GetViewMatrix();
	auto         Projection     = Camera.GetProjectionMatrix();
	auto         ViewProjection = Projection * View;

	// NOTE: to avoid matrix transpose need follow specific order of arguments in mul function in HLSL
	auto         cb             = CreateCBuffer<HLSL_PerDrawCB>();

	m_Shader->Bind();
	for (auto& p : m_Nodes)
	{
		glm::mat4 transform(1);
		//transform   = glm::translate(transform, {0, 1, 0});
		transform = glm::scale(transform, glm::vec3(SV_Scale));
		transform = glm::translate(transform, p.Pos);

		auto pos  = glm::vec4(p.Pos, 1) * transform;
		auto d    = glm::distance(Camera.GetPos(), glm::vec3(pos));
		if (d > CV_DrawDistance)
		{
			continue;
		}

		cb.World = transform;
		cb.MVP   = ViewProjection * cb.World;
		cb.MV    = View* cb.World;
		cb.Model = cb.World;

		ID3DBuffer* pBuffers[] = {
		    cb.Buffer.Get(),
		};

		cb.Write();

		::GetDeviceContext()->VSSetConstantBuffers(PERDRAW_SLOT, 1, pBuffers);
		Env::Renderer()->SetTexture(p.Albedo->GetTextureID(), eTT_Base);
		Env::Renderer()->SetTexture(p.Height->GetTextureID(), eTT_Heightmap);

		Env::Renderer()->DrawBuffer(m_pVerts.get(), &m_pIndices, m_pIndices.m_nItems, 0, static_cast<int>(RenderPrimitive::TRIANGLES), 0, 0, (CMatInfo*)-1);
	}
}

void CTerrainRenderer::Update()
{
	if (m_bNeedRegenerate)
	{
		m_pVerts.reset(nullptr);
		Env::Renderer()->ReleaseIndexBuffer(&m_pIndices);

		GenerateMesh(CV_TerrainPatchSize);

		m_bNeedRegenerate = false;
	}
}

void CTerrainRenderer::DrawAxises()
{
	auto       redColor   = Legacy::Vec3(0, 0, 1);
	auto       greenColor = Legacy::Vec3(0, 1, 0);
	auto       blueColor  = Legacy::Vec3(1, 0, 0);
	float      axisLength = 100.f;

	const auto oX         = Legacy::Vec3{1, 0, 0};
	const auto oY         = Legacy::Vec3{0, 1, 0};
	const auto oZ         = Legacy::Vec3{0, 0, 1};

	const auto A          = Legacy::Vec3{0, 1, 0};
	const auto B          = Legacy::Vec3{0, 0, 1};
	const auto C          = Legacy::Vec3{0, 1, 0};

	auto       dir        = Legacy::Vec3{1, 0, 0};
	//Nick(oX, A, -axisLength, axisLength, 1.f);
	//Nick(oY, B, -axisLength, axisLength, 1.f);
	//Nick(oZ, C, -axisLength, axisLength, 1.f);

	Env::AuxGeomRenderer()->DrawLine({0, -axisLength, 0}, {blueColor}, {0, axisLength, 0}, {blueColor});
	Env::AuxGeomRenderer()->DrawLine({-axisLength, 0, 0}, {redColor}, {axisLength, 0, 0}, {redColor});
	Env::AuxGeomRenderer()->DrawLine({0, 0, -axisLength}, {greenColor}, {0, 0, axisLength}, {greenColor});
}
void CTerrainRenderer::Nick(Legacy::Vec3 dir, Legacy::Vec3 normal, float from, float to, float step)
{
	auto  color  = UCol(Legacy::Vec3(1, 1, 1) * 0.15f);
	auto  v      = glm::normalize(glm::cross(dir, normal));
	auto  length = glm::length(to - from);
	float width  = 20;

	for (auto origin = from; origin < length; origin += step)
	{
		auto p = dir + dir * origin;

		Env::AuxGeomRenderer()->DrawLine(p + width * v, color, p - width * v, color);
	}
}

template<typename T>
T* VB(int nVerts)
{
	return (T*)CreateVertexBuffer(vertex_type<T>::value, nVerts);
}

template<typename T>
std::unique_ptr<T[]> MakeUniqueBuffer(int nVerts)
{
	static_assert(is_valid_vertex_type<T>::value, "This type is not Vertex Format");
	return std::make_unique<T[]>(nVerts);
}

void CTerrainRenderer::GenerateMesh(int size)
{
	using Legacy::Vec2;
	using Legacy::Vec3;
	struct Face
	{
		using index_type = uint16;
		index_type  v0, v1, v2;
		index_type& operator[](int i) { return (&v0)[i]; }
		index_type  operator[](int i) const { return (&v0)[i]; }
	};

	auto PatchSize = size;
	auto nVerts    = (PatchSize) * (PatchSize);
	auto Verts     = MakeUniqueBuffer<VertexType>(nVerts);
	auto nFaces    = 2 * (PatchSize - 1) * (PatchSize - 1);
	//auto Normals = DEBUG_NEW Vec3[3* PatchSize * PatchSize];
	auto Indices   = DEBUG_NEW Face[nFaces];
	for (int y = 0; y < PatchSize; y++)
	{
		for (int x = 0; x < PatchSize; x++)
		{
			auto idx       = y * PatchSize + x;
			Verts[idx].xyz = Vec3(float(x) / (PatchSize - 1), 0, float(y) / (PatchSize - 1));
			Verts[idx].st  = Vec2(float(x) / (PatchSize - 1), float(y) / (PatchSize - 1));
		}
	}
	int idx = 0;
	for (int y = 0; y < PatchSize - 1; y++)
	{
		for (int x = 0; x < PatchSize - 1; x++)
		{
			/*

			idx1----idx2
			|          |
			|		   |
			idx3----idx4	
			*/
			auto& f1   = Indices[idx++];
			auto& f2   = Indices[idx++];
			auto  idx1 = (y + 0) * (PatchSize - 0) + x + 0;
			auto  idx2 = (y + 0) * (PatchSize - 0) + x + 1;
			auto  idx3 = (y + 1) * (PatchSize - 0) + x + 0;
			auto  idx4 = (y + 1) * (PatchSize - 0) + x + 1;

			f1.v0      = idx1;
			f1.v1      = idx2;
			f1.v2      = idx3;

			f2.v0      = idx2;
			f2.v1      = idx4;
			f2.v2      = idx3;

			// idx1->idx2->idx3; idx2->idx4->idx3;
			//Face
		}
	}

	m_pVerts.reset(Env::Renderer()->CreateBuffer(nVerts, vertex_type<VertexType>::value, "TerrainPatch", true));
	Env::Renderer()->UpdateBuffer(m_pVerts.get(), Verts.get(), nVerts, true);

	Env::Renderer()->CreateIndexBuffer(&m_pIndices, Indices, 3 * nFaces);

	SAFE_DELETE_ARRAY(Indices);
}

void CTerrainRenderer::LoadTerrain(std::string_view baseFolder)
{
	char* diffuse_tmp = "Terrain/%s/diffuse_4097_x0%d_y0%d.dds";
	char* height_tmp  = "Terrain/%s/height_4097_x0%d_y0%d.dds";

	char  buffer[256] = {};
	int   nx          = 10;
	int   ny          = 10;
	for (int y = 0; y < ny; y++)
	{
		for (int x = 0; x < nx; x++)
		{
			sprintf(buffer, diffuse_tmp, baseFolder.data(), x, y);
			auto Albedo = Env::Renderer()->EF_LoadTexture(buffer, FT_NOREMOVE, 0, eTT_Base);
			sprintf(buffer, height_tmp, baseFolder.data(), x, y);
			auto  Height = Env::Renderer()->EF_LoadTexture(buffer, FT_NOREMOVE, 0, eTT_Heightmap);

			CTerrainNode patch{Albedo, Height, glm::vec3(x, 0, ny - y)};
			m_Nodes.emplace_back(patch);
		}
	}
}

bool CTerrainRenderer::OnBeforeVarChange(ICVar* pVar, const char* sNewValue)
{
	if (!strcmp(pVar->GetName(), "r_TerrainPatchSize"))
	{
		m_bNeedRegenerate = true;
		return true;
	}
	return true;
}

void CTerrainRenderer::OnAfterVarChange(ICVar* pVar)
{
}

void CTerrainRenderer::OnVarUnregister(ICVar* pVar)
{
}
