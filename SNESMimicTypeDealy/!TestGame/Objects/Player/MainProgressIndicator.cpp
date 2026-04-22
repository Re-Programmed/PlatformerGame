#include "MainProgressIndicator.h"

namespace GAME_NAME::Objects::Player
{
#define PROGRESS_BAR_HEIGHT 1.f
#define PROGRESS_BAR_SPRITE SpriteBase(382)

	using namespace GAME_NAME::Objects::GUI;

	std::unique_ptr<ProgressBar> MainProgressIndicator::m_progressBar = nullptr;

	void MainProgressIndicator::UpdateProgressIndicator(double progress)
	{
		if (m_progressBar == nullptr)
		{
			//Don't create it if we are just going to destroy it.
			if (progress >= 1.0 || progress <= 0) { return; }

			m_progressBar = std::make_unique<ProgressBar>(Vec2{ 0.f }, Vec2{ TargetResolutionX, PROGRESS_BAR_HEIGHT }, Renderer::GetSpriteIdFromTextureId(PROGRESS_BAR_SPRITE));
			Renderer::LoadGUIElement(m_progressBar.get(), 0);
		}

		if (progress >= 1.0 || progress <= 0)
		{
			Renderer::UnloadGUIElement(m_progressBar.get(), 0);
			m_progressBar.release();
			m_progressBar = nullptr;
		}
		else {
			m_progressBar->SetPercentage(progress * 100, true);
		}

	}

}
