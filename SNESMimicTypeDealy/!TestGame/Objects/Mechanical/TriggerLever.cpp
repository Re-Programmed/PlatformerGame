#include "TriggerLever.h"

#include "../../InputDisplay/DisplayIconManager.h"
#include "./MechanicalSaveManager.h"

namespace GAME_NAME::Objects::Mechanical
{
	TriggerLever::TriggerLever(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, bool leverOn, Triggerable* effect, std::string hoverDescription)
		: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, scale.Y * 2.f, position, scale, sprite), Trigger(effect), m_leverOn(leverOn), m_hoverDescription(hoverDescription)
	{
		MechanicalSaveManager::UpdateObject(this);
	}

	TriggerLever::~TriggerLever()
	{
		MechanicalSaveManager::UnregisterObject(this);
	}

	void TriggerLever::Update(GLFWwindow* window)
	{
		if (GetEffect() != nullptr)
		{
			trigger(!m_leverOn);
		}

		Interactable::Update(window);
	}

	void TriggerLever::Decode(const SaveParam param)
	{
		bool leverShouldBeOn = param.starts_with("t");

		if (!m_leverOn && leverShouldBeOn)
		{
			m_leverOn = true;
			m_sprite->SetSpriteId(m_sprite->GetSpriteId() + 1);
		}
		else if (m_leverOn && !leverShouldBeOn)
		{
			m_leverOn = false;
			m_sprite->SetSpriteId(m_sprite->GetSpriteId() - 1);
		}
	}

	MiscState::SaveParam TriggerLever::Encode()
	{
		return m_leverOn ? "t" : "f";
	}

	void TriggerLever::onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state)
	{
		Input::DisplayIconManager::ShowKeyInputDisplay(keyRef::PLAYER_INTERACT, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), state & InputManager::KEY_STATE_HELD ? 8 : 0, m_hoverDescription);

		if (state & InputManager::KEY_STATE_PRESSED)
		{
			m_leverOn = !m_leverOn;
			MechanicalSaveManager::UpdateObject(this);

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
