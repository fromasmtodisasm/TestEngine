#include "Terrain.h"

#include <BlackBox/3DEngine/IStatObj.hpp>
#include <BlackBox/Renderer/IRenderAuxGeom.hpp>

#include "StatObject.hpp"
#include "CryHeaders.h"

CTerrain::CTerrain()
{
	GenerateMesh();

	m_Shader  = Env::Renderer()->Sh_Load("terrain");
	m_Texture = Env::Renderer()->EF_LoadTexture("Terrain/GrandCanyon/diffuse_4097_x00_y00.png", FT_NOREMOVE, 0, eTT_Base);

	m_pRendElement = Env::Renderer()->EF_CreateRE(EDataType::eDATA_Terrain);

}

CTerrain::~CTerrain()
{
	//Env::Renderer()->ReleaseBuffer(m_pVerts);
	SAFE_DELETE(m_pRendElement);
}

void CTerrain::Render(CCamera& Camera)
{
	return;

	DrawAxises();

#if 0
	for each (const auto& area in m_Areas)
	{
		SRendParams rp;
		area->Render(rp, {});
	}
#endif
}

void CTerrain::Update()
{
}

void CTerrain::DrawAxises()
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
	Nick(oX, A, -axisLength, axisLength, 1.f);
	//Nick(oY, B, -axisLength, axisLength, 1.f);
	Nick(oZ, C, -axisLength, axisLength, 1.f);

	Env::AuxGeomRenderer()->DrawLine({0, -axisLength, 0}, {blueColor}, {0, axisLength, 0}, {blueColor});
	Env::AuxGeomRenderer()->DrawLine({-axisLength, 0, 0}, {redColor}, {axisLength, 0, 0}, {redColor});
	Env::AuxGeomRenderer()->DrawLine({0, 0, -axisLength}, {greenColor}, {0, 0, axisLength}, {greenColor});
}
void CTerrain::Nick(Legacy::Vec3 dir, Legacy::Vec3 normal, float from, float to, float step)
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

const int PatchSize = 512;
void      CTerrain::GenerateMesh()
{
	using Legacy::Vec2;
	using Legacy::Vec3;

	auto nVerts = PatchSize * PatchSize;
	auto Verts  = MakeUniqueBuffer<VertexType>(nVerts);
	//auto Normals = DEBUG_NEW Vec3[3* PatchSize * PatchSize];
	//auto Indices = DEBUG_NEW CryFace[PatchSize * PatchSize];
	for (int y = 0; y < PatchSize; y++)
	{
		for (int x = 0; x < PatchSize; x++)
		{
			auto idx       = y * PatchSize + x;
			Verts[idx].xyz = Vec3(float(x) / PatchSize, 0, float(y) / PatchSize);
			Verts[idx].st  = Vec2(float(x) / PatchSize, float(y) / PatchSize);
		}
	}

	m_pVerts.reset(Env::Renderer()->CreateBuffer(nVerts, vertex_type<VertexType>::value, "TerrainPatch", true));
}
