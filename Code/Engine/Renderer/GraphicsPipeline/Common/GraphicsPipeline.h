#pragma once

struct IRenderView;

class CGraphicsPipelineStage;

class CGraphicsPipeline
{
public:
	CGraphicsPipeline() = default;
	void Execute(IRenderView* pRenderView);

	std::vector<CGraphicsPipelineStage*> m_PipelineStages;
};