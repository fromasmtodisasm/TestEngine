#pragma once

class CCamera;

struct IEntityRenderer;

struct IRenderView
{
	virtual const CCamera& GetCamera() = 0;
	virtual void           AddEntityRenderer(IEntityRenderer* pEntityRenderer) = 0;
};