#pragma once
#include "IRender.hpp"

struct SRender2DImageDescription;

struct IRenderAuxGeom
{
	using Vec3 = Legacy::Vec3;
	using Vec2 = Legacy::Vec2;

	virtual void           DrawAABB(Vec3 min, Vec3 max, const UCol& col)                                                                                 = 0;
	virtual void           DrawTriangle(const Vec3& v0, const UCol& colV0, const Vec3& v1, const UCol& colV1, const Vec3& v2, const UCol& colV2) = 0;
	virtual void           DrawLine(const Vec3& v0, const UCol& colV0, const Vec3& v1, const UCol& colV1, float thickness = 1.0f)                        = 0;
	virtual void           DrawLines(const Vec3* v, uint32 numPoints, const UCol& col, float thickness = 1.0f)                                                   = 0;
	//! Adds a 2D image that should be drawn on the screen to an internal render list.
	//! The function supports placing images in stereo 3D space.
	//! The stereo params are the same that are used for the scene. A value of 0 is handled as a special case and always places the image on the screen plane.
	//! image.stereoDepth - Places image in stereo 3d space. The depth is specified in camera space.
	virtual void           PushImage(const SRender2DImageDescription& image)                                                                                             = 0;
	static IRenderAuxGeom* GetAux()
	{
		return Env::Renderer() ? Env::Renderer()->GetIRenderAuxGeom() : Env::AuxGeomRenderer();
	}

	//! Draw Text.
	//! ##@{
	virtual void RenderTextQueued(Vec3 pos, const SDrawTextInfo& ti, const char* text) = 0;

	void         DrawQuad(const Vec3& v0, const UCol& colV0, const Vec3& v1, const UCol& colV1, const Vec3& v2, const UCol& colV2, const Vec3& v3, const UCol& colV3)
	{
		DrawTriangle(v0, colV0, v1, colV1, v2, colV2);
		DrawTriangle(v0, colV0, v2, colV2, v3, colV3);
	}

	virtual void DrawMesh(CVertexBuffer* pVertexBuffer, SVertexStream* pIndices, glm::mat4 transform, int texture) = 0;
	virtual void Flush()                                                                                           = 0;

};
class IRenderAuxText
{
public:
	using Vec3 = Legacy::Vec3;
	using Vec2 = Legacy::Vec2;

	struct AColor
	{
		float rgba[4];

		static const float* white()
		{
			static float col[] = { 1, 1, 1, 1 };
			return col;
		}

		AColor(const float* ptr)
		{
			const float* col = ptr ? ptr : white();

			rgba[0] = col[0];
			rgba[1] = col[1];
			rgba[2] = col[2];
			rgba[3] = col[3];
		}

		AColor(const ColorF& cf)
		{
			rgba[0] = cf.r;
			rgba[1] = cf.g;
			rgba[2] = cf.b;
			rgba[3] = cf.a;
		}

		AColor(const Vec3& v)
		{
			rgba[0] = v.x;
			rgba[1] = v.y;
			rgba[2] = v.z;
			rgba[3] = 1;
		}

		//AColor(const ColorB& cb)
		//{
		//	rgba[0] = cb.r / 255.0f;
		//	rgba[1] = cb.g / 255.0f;
		//	rgba[2] = cb.b / 255.0f;
		//	rgba[3] = cb.a / 255.0f;
		//}

		AColor(float r, float g, float b, float a)
		{
			rgba[0] = r;
			rgba[1] = g;
			rgba[2] = b;
			rgba[3] = a;
		}
	};

	struct ASize
	{
		Vec2 val;

		ASize(float f)          : val(f)    {}
		ASize(float x, float y) : val(x, y) {}
	};

	static void DrawText(Vec3 pos, const SDrawTextInfo& ti, const char* text)
	{
		IRenderAuxGeom::GetAux()->RenderTextQueued(pos, ti, text);
	}

	static void DrawText(Vec3 pos, const ASize& size, const AColor& color, int flags, const char* text)
	{
		if (text && !Env::Get()->IsDedicated())
		{
			SDrawTextInfo ti;
			#if 0
			ti.scale = size.val;
			#endif
			ti.flags = flags;
			ti.color[0] = color.rgba[0];
			ti.color[1] = color.rgba[1];
			ti.color[2] = color.rgba[2];
			ti.color[3] = color.rgba[3];

			DrawText(pos, ti, text);
		}
	}

	static void DrawText(Vec3 pos, const ASize& size, const AColor& color, int flags, const char* format, va_list args)
	{
		if( format )
		{
			char str[512];

			vsprintf(str, format, args);

			DrawText(pos, size, color, flags, str);
		}
	}

	static void DrawTextF(Vec3 pos, const ASize& size, const AColor& color, int flags, const char* label_text, ...) PRINTF_PARAMS(5, 6)
	{
		va_list args;
		va_start(args, label_text);
		DrawText(pos, size, color, flags, label_text, args);
		va_end(args);
	}

	static void DrawLabel(Vec3 pos, float font_size, const char* text)
	{
		DrawText(pos, font_size, AColor::white(), eDrawText_FixedSize, text);
	}

	static void DrawLabelF(Vec3 pos, float font_size, const char* label_text, ...) PRINTF_PARAMS(3, 4)
	{
		va_list args;
		va_start(args, label_text);
		DrawText(pos, font_size, AColor::white(), eDrawText_FixedSize, label_text, args);
		va_end(args);
	}

	static void DrawLabelEx(Vec3 pos, float font_size, const AColor& color, bool bFixedSize, bool bCenter, const char* text)
	{
		DrawText(pos, font_size, color, ((bFixedSize) ? eDrawText_FixedSize : 0) | ((bCenter) ? eDrawText_Center : 0), text);
	}

	static void DrawLabelExF(Vec3 pos, float font_size, const AColor& color, bool bFixedSize, bool bCenter, const char* label_text, ...) PRINTF_PARAMS(6, 7)
	{
		va_list args;
		va_start(args, label_text);
		DrawText(pos, font_size, color, ((bFixedSize) ? eDrawText_FixedSize : 0) | ((bCenter) ? eDrawText_Center : 0), label_text, args);
		va_end(args);
	}

	static void Draw2dLabelEx(float x, float y, float font_size, const AColor& color, EDrawTextFlags flags, const char* label_text, ...) PRINTF_PARAMS(6, 7)
	{
		va_list args;
		va_start(args, label_text);
		DrawText(Vec3(x, y, 0.5f), font_size, color, flags, label_text, args);
		va_end(args);
	}

	static void Draw2dLabel(float x, float y, float font_size, const AColor& color, bool bCenter, const char* label_text, ...) PRINTF_PARAMS(6, 7)
	{
		va_list args;
		va_start(args, label_text);
		DrawText(Vec3(x, y, 0.5f), font_size, color, eDrawText_2D | eDrawText_800x600 | eDrawText_FixedSize | ((bCenter) ? eDrawText_Center : 0), label_text, args);
		va_end(args);
	}


	static void Draw2dText(float posX, float posY, const AColor& color, const char* pStr)
	{
		DrawText(Vec3(posX, posY, 1.f), 1, color, eDrawText_LegacyBehavior, pStr);
	}

	static void PrintToScreen(float x, float y, float size, const char* buf)
	{
		DrawText(Vec3(x, y, 1.f), ASize(size * 0.5f / 8, size * 1.f / 8), AColor::white(), eDrawText_800x600 | eDrawText_2D | eDrawText_LegacyBehavior, buf);
	}

	static void WriteXY(int x, int y, float xscale, float yscale, float r, float g, float b, float a, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		DrawText(Vec3(float(x), float(y), 1.f), ASize(xscale, yscale), AColor(r, g, b, a), eDrawText_800x600 | eDrawText_2D | eDrawText_LegacyBehavior, format, args);
		va_end(args);
	}

	static void TextToScreenColor(int x, int y, float r, float g, float b, float a, const char* text)
	{
		WriteXY((int)(8 * x), (int)(6 * y), 1, 1, r, g, b, a, "%s", text);
	}

	static void TextToScreen(float x, float y, const char* text)
	{
		TextToScreenColor((int)(8 * x), (int)(6 * y), 1, 1, 1, 1, text);
	}

	static void TextToScreenF(float x, float y, const char* format, ...)
	{
		char buffer[512];
		va_list args;
		va_start(args, format);
		vsprintf(buffer, format, args);
		va_end(args);

		TextToScreenColor((int)(8 * x), (int)(6 * y), 1, 1, 1, 1, buffer);
	}
};


// 2D Images added to the rendering frame
struct SRender2DImageDescription
{
	float        x = 0;
	float        y = 0;
	float        z = 0;
	float        w = 0;
	float        h = 0;
	Legacy::Vec2 uv[2]; //!< Texture UV coordinates
	float        angle       = 0;
	float        stereoDepth = 0; //!< Places image in stereo 3d space. The depth is specified in camera space.
	UCol         color;

	uint32       textureId = 0;
	uint32       targetId  = 0;

	// SDisplayContextKey displayContextKey;
	// SAuxGeomRenderFlags renderFlags = e_Def2DImageRenderflags;
};

// Helper class to abstract pushing 2d images for rendering
class IRenderAuxImage
{
	struct S2DImage
	{
		float        x = 0;
		float        y = 0;
		float        z = 0;
		float        w = 0;
		float        h = 0;
		Legacy::Vec2 uv[2]; // Texture UV coordinates
		float        angle       = 0;
		float        stereoDepth = 0;
		ColorB       color;

		uint32       textureId = 0;
		uint32       targetId  = 0;
	};

public:
	static void Draw2dImage(float xpos, float ypos, float w, float h, int texture_id, float s0 = 0, float t0 = 0, float s1 = 1, float t1 = 1, float angle = 0,
	                        float r = 1, float g = 1, float b = 1, float a = 1, float z = 1, float stereoDepth = 0)
	{
		if (Env::Renderer())
		{
			SRender2DImageDescription img;
			img.x           = xpos;
			img.y           = ypos;
			img.z           = z;
			img.w           = w;
			img.h           = h;
			img.textureId   = texture_id;
			// img.targetId = 0;
			img.uv[0].x     = s0;
			img.uv[0].y     = t0;
			img.uv[1].x     = s1;
			img.uv[1].y     = t1;
			img.angle       = angle;
			img.color       = UCol((uint8)(r * 255), (uint8)(r * 255), (uint8)(r * 255), (uint8)(r * 255));
			img.stereoDepth = stereoDepth;

			IRenderAuxGeom::GetAux()->PushImage(img);
		}
	}
	static void Draw2dImage(float xpos, float ypos, float w, float h, int texture_id, float s0, float t0, float s1, float t1, float angle, const ColorF& col)
	{
		if (Env::Renderer())
		{
			SRender2DImageDescription img;
			img.x           = xpos;
			img.y           = ypos;
			img.z           = 1.f;
			img.w           = w;
			img.h           = h;
			img.textureId   = texture_id;
			// img.targetId = 0;
			img.uv[0].x     = s0;
			img.uv[0].y     = t0;
			img.uv[1].x     = s1;
			img.uv[1].y     = t1;
			img.angle       = angle;
			img.color       = UCol(col);
			img.stereoDepth = 0.f;

			IRenderAuxGeom::GetAux()->PushImage(img);
		}
	}
	//! Draws a image using the current matrix.
	static void DrawImage(float xpos, float ypos, float w, float h, int texture_id, float s0, float t0, float s1, float t1, float r, float g, float b, float a, bool filtered = true)
	{
		if (Env::Renderer())
		{
			SRender2DImageDescription img;
			img.x           = xpos;
			img.y           = ypos;
			img.z           = 1.f;
			img.w           = w;
			img.h           = h;
			img.textureId   = texture_id;
			// img.targetId = 0;
			img.uv[0].x     = s0;
			img.uv[0].y     = t0;
			img.uv[1].x     = s1;
			img.uv[1].y     = t1;
			img.angle       = 0.f;
			img.color       = UCol((uint8)(r * 255), (uint8)(r * 255), (uint8)(r * 255), (uint8)(r * 255));
			img.stereoDepth = 0.f;

			IRenderAuxGeom::GetAux()->PushImage(img);
		}
	}
};
