#include "DeathEventManager.h"

#include "../Objects/GUI/GUIButton.h"
#include "../Objects/GUI/GUIManager.h"

#include "../Objects/GUI/Text/TextRenderer.h"
#include "Level/LevelManager.h"
#include "Objects/StartState.h"

namespace GAME_NAME
{
	Vec2 DeathEventManager::m_playerFlaggedPosition = Vec2{ 0.f, 0.f };
	std::unique_ptr<DeathEventManager> DeathEventManager::UPDATE_INSTANCE = nullptr;

	unsigned int DeathEventManager::m_retryButtonID;

	void DeathEventManager::ReloadFromSave()
	{
		//Load data from the last created start state.
		StartState::SetToBeLoaded();

		TestGame::INSTANCE->SetLoadLevelWithSavedPlayer(true);
		TestGame::INSTANCE->LoadLevelAndAllData(TestGame::GetGlobalLevelData()->GetLevelPath().c_str());
	}

	void DeathEventManager::ShowDeathScreen(DEATH_SCREEN_TYPE screenType)
	{
		Vec2 scale = Vec2{ 128.f, 32.f };
		Sprite* sp = Renderer::GetSprite(SpriteBase(-1));
		GUIButton* retry = new GUIButton(Vec2{ TargetResolutionX / 2.f, TargetResolutionY / 2.f - 40.f } - scale / 2.f, scale, sp->GetSpriteId(), new std::function<void(int)>(deathButtonEvents), 0, Vec4{ 1.f, 1.f, 1.f, 1.f }, Vec4{
			1.f, 0.3f, 0.3f, 1.f
		});
		delete sp;

		GUIManager::MenuIsOpen = true;
		Renderer::LoadGUIElement(retry, 2);
		m_retryButtonID = GUIManager::RegisterButton(retry);
	
		Text::TextRenderer::RenderedWord retryWord = Text::TextRenderer::RenderWord("Retry", retry->GetPosition() + Vec2{30.f, 3.f}, 24.f, -1.f, 2);

	}

	void DeathEventManager::Update(GLFWwindow* window)
	{
		if (LevelManager::GetCircleAnimationCompleted())
		{
			GUIManager::MenuIsOpen = false;

			GUIManager::UnregisterButton(m_retryButtonID);

			LevelManager::SetCircleAnimationCompleted(false);

			ReloadFromSave();

			//LevelManager::LevelCircleAnimation({ -1, -1 }, true);

			//this->DisableUpdates(); (Happens automatically upon destruction)

			//Remove update instance to stop checking for level load.
			UPDATE_INSTANCE = nullptr;
		}
	}

	void DeathEventManager::deathButtonEvents(int buttonID)
	{
		//Retry button.
	
	
		//Already loading something.
		if (UPDATE_INSTANCE != nullptr) { return; }

		LevelManager::LevelCircleAnimation({ -1, -1 });

		//Set the update function to begin by creating a new instance of the class.
		UPDATE_INSTANCE = std::make_unique<DeathEventManager>();
	}
}
