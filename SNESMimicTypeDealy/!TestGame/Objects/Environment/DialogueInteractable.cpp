#include "DialogueInteractable.h"

#include "../../InputDisplay/DisplayIconManager.h"
#include "../../../Objects/GUI/Menus/GUIMenu.h"


namespace GAME_NAME::Objects::Environment
{
	DialogueInteractable::DialogueInteractable(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, std::string dialogueSource)
		: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE::KEY_STATE_NONE, (m_scale.X > m_scale.Y ? m_scale.X : m_scale.Y) + 14.f, position, scale, sprite), m_dialogueSource(dialogueSource)
	{
		
	}

	void DialogueInteractable::onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state)
	{	
		if (state & InputManager::KEY_STATE_RELEASED)
		{
			if (!GUI::Menus::GUIMenu::MenuIsOpen())
			{
				GUI::Menus::GUIMenu::OpenMenu();
			}

			Cutscenes::DialogueManager::INSTANCE->PlayDialogueSequence(Cutscenes::DialogueManager::INSTANCE->GetDialogueSequence(m_dialogueSource));
		}

		GAME_NAME::Input::DisplayIconManager::ShowKeyInputDisplay(keyRef::PLAYER_INTERACT, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), state & InputManager::KEY_STATE_HELD ? 8 : 0);
	}
}

