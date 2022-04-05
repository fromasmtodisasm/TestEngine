#include <BlackBox/3DEngine/I3DEngine.hpp>
#include <BlackBox/Profiler/Profiler.h>
#include "System.hpp"
#include "System.inl"

#include "CrySizerImpl.h"

void CSystem::Render()
{
	if (m_env.pRenderer)
	{
		if (m_pProcess->GetFlags() & PROC_3DENGINE)
		{
			PROFILER_PUSH_CPU_MARKER("CPU RENDER", Utils::COLOR_YELLOW);
			{
				m_env.pRenderer->SetState(IRenderer::State::DEPTH_TEST, true);
				m_env.p3DEngine->SetCamera(GetViewCamera());
				m_env.p3DEngine->Draw();
			}
			PROFILER_POP_CPU_MARKER();
		}
	}
}

void CSystem::RenderBegin()
{
	if (!m_env.pRenderer)
		return;
	PROFILER_SYNC_FRAME();
	PROFILER_PUSH_CPU_MARKER("Full frame", COLOR_GRAY);
	m_env.pRenderer->BeginFrame();
}
void CSystem::OnRenderer_BeforeEndFrame()
{
	RenderStats();
}

void MemoryUsageHeader(IFFont* pFont, float px, float& py)
{
	static char stats[256];
	auto        toset = 15 + 14 + 1;

	memset(stats, '-', toset);
	stats[toset] = 0;
	PrintRightAlignedText(py, stats, pFont);
	py += 18;

	auto len = sprintf(stats, "%-15.15s %-14.14s", "$5Module", "  Allocated");

	PrintRightAlignedText(py, stats, pFont);
	py += 18;
}

void CSystem::RenderStats()
{
	if (m_env.pRenderer)
	{
		if (sys_dump_memstats)
		{
			auto pFont = m_env.pCryFont->GetFont("default");

			if (m_env.pRenderer->GetFrameID(false) % 1000)
				return;
			static int lastFrame = Env::Renderer()->GetFrameID();
#if 1
			float px               = 100;
			float py               = 200;
			float dy               = 18;
			auto  PrintMemoryUsage = [&, this](const char* name, typename auto fn, auto This)
			{
				PrintMemoryUsageForName(name, pFont, fn, This, px, py);
				py += dy;
			};

			//PrintMemoryUsage("Game", &IGame::GetMemoryStatistics, m_pGame);
			MemoryUsageHeader(pFont, px, py);
			PrintMemoryUsage("System", &CSystem::GetMemoryUsage, this);
			PrintMemoryUsage("Renderer", &IRenderer::GetMemoryUsage, m_env.pRenderer);
			PrintMemoryUsage("ScriptSsystem", &IScriptSystem::GetMemoryUsage, m_env.pScriptSystem);
			PrintMemoryUsage("3DEngine", &I3DEngine::GetMemoryUsage, m_env.p3DEngine);
			PrintMemoryUsage("EntitySystem", &IEntitySystem::GetMemoryUsage, m_env.pEntitySystem);

			char stats[256];
			auto toset = 15 + 14 + 1;

			memset(stats, '-', toset);
			stats[toset] = 0;
			PrintRightAlignedText(py, stats, pFont);
#endif
		}
		//PrintRightAlignedText(Env::Renderer()->GetHeight() - 64.f, "$1BLACKBOX $8ENGINE", m_pBlackBoxFont);

		int iDisplayInfo = m_rDisplayInfo;
		if (iDisplayInfo == 0)
			return;
		// Draw engine stats
		if (m_env.p3DEngine)
		{
			// Draw 3dengine stats and get last text cursor position
			float nTextPosX = 101 - 20, nTextPosY = 20, nTextStepY = 3;
			m_env.p3DEngine->DisplayInfo(nTextPosX, nTextPosY, nTextStepY);

#if defined(ENABLE_LW_PROFILERS)
			if (m_rDisplayInfo->GetIVal() == 2)
			{
				IRenderAuxText::TextToScreenF(nTextPosX, nTextPosY += nTextStepY, "SysMem %.1f mb", float(DumpMMStats(false)) / 1024.f);
			}
#endif
		}
	}
}

void CSystem::RenderEnd()
{
	if (!m_env.pRenderer)
		return;
	PROFILER_POP_CPU_MARKER();
	{
		//DEBUG_GROUP("DRAW_PROFILE");
		PROFILER_DRAW();
	}
	if (m_env.pRenderer)
	{
		m_env.pRenderer->Update();
	}
}
