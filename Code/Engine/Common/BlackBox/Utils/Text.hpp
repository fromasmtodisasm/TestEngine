#pragma once
#include <BlackBox/Renderer/IRenderAuxGeom.hpp>

inline void PrintRightAlignedText(float posY, const char* szText, IFFont* pFont = nullptr)
{
	using Legacy::Vec3;

	SDrawTextInfo info;
	float         rightMargin = 20;
	info.font                 = pFont;
#if 0
		auto& color = info.color;
		color[0]	= 1.0; //green
		color[1]	= 1.0;
		color[2]	= 1.0; //alpha
		color[3]	= 0.0; //red
#endif
	//Env::Renderer()->Draw2dText(Env::Renderer()->GetWidth() - info.font->TextWidth(szText) - rightMargin, posY, szText, info);
	auto x = Env::Renderer()->GetWidth() - info.font->GetTextSize(szText).x - rightMargin;
	IRenderAuxText::DrawText(Vec3(x, posY, 0), info, szText);
}
