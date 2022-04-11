#pragma once
#include <BlackBox/System/IStreamEngine.h>
#include <BlackBox/System/ConsoleRegistration.h>

class CStatObj;
class CIndexedMesh;

#define DeclareUnique(T, F)                                   \
	class T##Unique : public std::unique_ptr<T, void (*)(T*)> \
	{                                                         \
	public:                                                   \
		using Base = T;                                       \
		using Uniq = std::unique_ptr<T, void (*)(T*)>;        \
		T##Unique(Base* ptr = nullptr)                        \
		    : Uniq(ptr, [](T* b)                              \
		           { { F(b); }; })                                       \
		{                                                     \
		}                                                     \
	};
/////////////////////////////////////////////////////
DeclareUnique(CVertexBuffer, (Env::Renderer()->ReleaseBuffer));
/////////////////////////////////////////////////////
DeclareUnique(SVertexStream, (Env::Renderer()->ReleaseIndexBuffer));

template<typename T>
inline CVertexBufferUnique CreateVertexBuffer(int vertexCount, int vertexFormat, const char* szSource, bool bDynamic = false)
{
	return CVertexBufferUnique(
	    Env::Renderer()->CreateBuffer(
	        vertexCount, vertex_type<T>::value, szSource, bDynamic),
	    [](CVertexBuffer* b)
	    { Env::Renderer()->ReleaseBuffer(b); });
}
/////////////////////////////////////////////////////
using ShaderPtr  = _smart_ptr<IShader>;
using TexturePtr = _smart_ptr<ITexPic>;

class CTerrainNode
{
public:
	TexturePtr Albedo = nullptr;
	TexturePtr Height = nullptr;
	glm::vec3  Pos;
};

template<typename T>
class CVar
{
public:
	CVar(cstr Name, T Val)
	    : m_Name(Name)
	    , m_Val(Val)
	{
		REGISTER_CVAR2(Name, &m_Val, Val, 0, "");
	}
	~CVar()
	{
		if (Env::Console())
		{
			auto var = Env::Console()->GetCVar(m_Name);
			SAFE_UNREGISTER_CVAR(var);
		}
	}
	operator T()
	{
		return m_Val;
	}

public:
	T    m_Val;
	cstr m_Name;
};

using CVarFloat = CVar<float>;
using CVarInt   = CVar<int>;

class CQuadTreeTerrain
{
public:
	CQuadTreeTerrain() = default;
};

class CTerrainRenderer : IConsoleVarSink
{
public:
	using Vec3          = Legacy::Vec3;
	using VertexType    = SVF_P3F_T2F;

	const int PatchSize = 65;

public:
	CTerrainRenderer();
	~CTerrainRenderer();

	void Render(CCamera& Camera);
	void PrepareForDrawing();
	void RenderNodes(CCamera& Camera);
	void Update();

	void DrawAxises();

	void Nick(Legacy::Vec3 dir, Legacy::Vec3 normal, float from, float to, float step = 1.f);

private:
	void GenerateMesh(int size);

	void LoadTerrain(std::string_view baseFolder);

public:
	//std::vector<_smart_ptr<CStatObj>> m_Areas;

	CVertexBufferUnique        m_pVerts;
	SVertexStream              m_pIndices;

	ShaderPtr                  m_Shader;
	CRendElement*              m_pRendElement;

	std::vector<CTerrainNode>  m_Nodes;
	//REGISTER_CVAR2("r_TerrainPatchSize", &CV_TerrainPatchSize, 64, 0, "");
	//REGISTER_CVAR2("r_DrawDistance", &CV_DrawDistance, 500.f, 0, "Terrain patch draw distance");
	//REGISTER_CVAR2("r_TerrainPatchScale", &CV_Scale, 100.f, 0, "Terrain patch scale");

	CVarInt                    CV_TerrainPatchSize{"r_TerrainPatchSize", 65};
	CVarFloat                  CV_DrawDistance{"r_DrawDistance", 500.f};
	CVarFloat                  CV_Scale{"r_TerrainPatchScale", 100.f};
	bool                       m_bNeedRegenerate{};
	ComPtr<ID3D11SamplerState> LinearSampler;

	// Inherited via IConsoleVarSink
	virtual bool               OnBeforeVarChange(ICVar* pVar, const char* sNewValue) override;
	virtual void               OnAfterVarChange(ICVar* pVar) override;
	virtual void               OnVarUnregister(ICVar* pVar) override;
};

extern std::unique_ptr<CTerrainRenderer> gTerrainRenderer;
