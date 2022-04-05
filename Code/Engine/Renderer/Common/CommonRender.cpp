#include "CommonRender.h"
#include <D3D/Renderer.h>
// NameTable internal variable.
CNameTableR* CCryNameR::ms_table;

namespace
{
	inline bool Check(HRESULT hr, const char* msg)
	{
		if (FAILED(hr))
		{
			CryError(msg);
			return false;
		}
		return true;
	}
} // namespace

#define CheckError(stmnt, msg)                  \
	do {                                        \
		if (!Check((stmnt), msg)) return false; \
	} while (false)

CGlobalResources::~CGlobalResources()
{
}

bool CGlobalResources::Valid()
{
	return m_Valid;
}

bool CGlobalResources::Init()
{
	D3D11_SAMPLER_DESC desc;
	ZeroStruct(desc);
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.Filter   = D3D11_FILTER(D3D11_FILTER_ANISOTROPIC | D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	CheckError(
	    GetDevice()->CreateSamplerState(&desc, LinearSampler.GetAddressOf()),
	    "Error create sampler for font");

	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));

	BlendState.RenderTarget[0].BlendEnable           = TRUE;
	BlendState.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	CheckError(
	    GetDevice()->CreateBlendState(&BlendState, FontBlendState.GetAddressOf()),
	    "Error create blend state");

	TexturedQuadShader = (CShader*)Env::Renderer()->Sh_Load("auxgeom.TexturedQuad", 0, 0);

	return true;
}

CGlobalResources::CGlobalResources()
{
	m_Valid = Init();
}
