#include "TriggerLever.h"

#include "../../InputDisplay/DisplayIconManager.h"

namespace GAME_NAME::Objects::Mechanical
{
	TriggerLever::TriggerLever(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, bool leverOn, Triggerable* effect)
		: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, scale.Y * 2.f, position, scale, sprite), Trigger(effect), m_leverOn(leverOn)
	{
		
	}

	void TriggerLever::Update(GLFWwindow* window)
	{
		if (GetEffect() != nullptr)
		{
			trigger(!m_leverOn);
		}

		Interactable::Update(window);
	}

	void TriggerLever::onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state)
	{
		Input::DisplayIconManager::ShowKeyInputDisplay(keyRef::PLAYER_INTERACT, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), state & InputManager::KEY_STATE_HELD ? 8 : 0);

		if (state & InputManager::KEY_STATE_PRESSED)
		{
			m_leverOn = !m_leverOn;

			if (m_leverOn)
			{
				m_sprite->SetSpriteId(m_sprite->GetSpriteId() + 1);
			}
			else {
				m_sprite->SetSpriteId(m_sprite->GetSpriteId() - 1);
			}
		}
	}
}
