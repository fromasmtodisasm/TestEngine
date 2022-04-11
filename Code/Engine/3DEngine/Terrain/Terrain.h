#pragma once

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

DeclareUnique(CVertexBuffer, (Env::Renderer()->ReleaseBuffer))

    template<typename T>
    inline CVertexBufferUnique CreateVertexBuffer(int vertexCount, int vertexFormat, const char* szSource, bool bDynamic = false)
{
	return CVertexBufferUnique(
	    Env::Renderer()->CreateBuffer(
	        vertexCount, vertex_type<T>::value, szSource, bDynamic),
	    [](CVertexBuffer* b)
	    { Env::Renderer()->ReleaseBuffer(b); });
}

class CTerrain
{
	using Vec3       = Legacy::Vec3;
	using VertexType = SVF_P3F_T2F;
	using ShaderPtr  = _smart_ptr<IShader>;
	using TexturePtr = _smart_ptr<ITexPic>;

public:
	CTerrain();
	~CTerrain();

	void Render(CCamera& Camera);
	void Update();

	void DrawAxises();

	void Nick(Legacy::Vec3 dir, Legacy::Vec3 normal, float from, float to, float step = 1.f);

private:
	void GenerateMesh();

private:
	//std::vector<_smart_ptr<CStatObj>> m_Areas;

	CVertexBufferUnique m_pVerts;
	ShaderPtr           m_Shader;
	TexturePtr          m_Texture = nullptr;
	CRendElement*       m_pRendElement = nullptr;
};
