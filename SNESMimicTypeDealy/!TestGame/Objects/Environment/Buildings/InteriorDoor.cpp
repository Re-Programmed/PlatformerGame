#include "InteriorDoor.h"

#include "../../../InputDisplay/DisplayIconManager.h"
#include "../../../Level/LevelManager.h"

#include "../../../Mappings.h"

#include "../../../../Utils/Time/GameTime.h"

#include "../../../../Objects/StateSaver.h"

#include "../../../../Resources/Save/SaveManager.h"

namespace GAME_NAME::Objects::Environment::Buildings
{
	InteriorDoor::InteriorDoor(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, std::string levelDestination, Vec2 exitPosition, bool loadsObjectsOnly, bool useLocalState)
		: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, position, scale, sprite), m_levelDestination(levelDestination), m_loadsObjectsOnly(loadsObjectsOnly), m_exitPosition(exitPosition), m_useLocalState(useLocalState)
	{

	}

	void InteriorDoor::onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state)
	{
		Input::DisplayIconManager::ShowKeyInputDisplay(Input::DisplayIconManager::INPUT_DISPLAY_KEY_E, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), state & InputManager::KEY_STATE::KEY_STATE_HELD ? 8 : 0);

		if (state & InputManager::KEY_STATE::KEY_STATE_RELEASED)
		{
			if (!m_used)
			{
				StateSaver::SaveStates();
				StateSaver::SaveMisc();

				TestGame::ThePlayer->SetFrozen(true, Player::Player::BEHIND);
				LevelManager::LevelCircleAnimation(Vec2{ -1 }, false, false);
				m_used = true;
			}
		}

	}

	void InteriorDoor::Update(GLFWwindow* window)
	{
		Interactable::Update(window);
		
		/*if (m_used)
		{
			TestGame::ThePlayer->Translate(Vec2{ 0.f, static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()) * 30.f });
		}*/

		//We were used and tried to load a level. Perform the loading based on if it should have the textures loaded as well.
		if (m_used && LevelManager::GetCircleAnimationCompleted())
		{
			LevelManager::SetCircleAnimationCompleted(false);
			
			if (m_loadsObjectsOnly)
			{
				Resources::SaveManager::SetCurrentLevelFile(Resources::SaveManager::GetCurrentFile() + m_levelDestination);
				TestGame::INSTANCE->LoadLevelOnlyObjects(m_levelDestination.c_str(), m_exitPosition);
			}
			else {

				if (m_useLocalState)
				{
					TestGame::INSTANCE->SetLoadLevelWithSavedPlayer(true);
				}

				TestGame::INSTANCE->LoadLevelAndAllData(m_levelDestination.c_str());
			}

			
		}

	}
}