#include "TriggerKeyHole.h"

#include "./MechanicalSaveManager.h"

#include "../../Items/FloorItem.h"

#include "../../InputDisplay/DisplayIconManager.h"

namespace GAME_NAME::Objects::Mechanical
{
	TriggerKeyHole::TriggerKeyHole(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, Triggerable* effect, bool keyRemovable)
		: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, scale.Y * 2.f, position, scale, sprite), Trigger(effect), m_keyRemovable(keyRemovable)
	{
		MechanicalSaveManager::UpdateObject(this);
	}

	TriggerKeyHole::TriggerKeyHole()
		: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, 0.f, Vec2{ 0.f }, Vec2{ 0.f }, nullptr), Trigger(nullptr), m_keyRemovable(true)
	{
		MechanicalSaveManager::UpdateObject(this);
	}

	TriggerKeyHole::~TriggerKeyHole()
	{
		MechanicalSaveManager::UnregisterObject(this);
	}

	void TriggerKeyHole::Update(GLFWwindow* window)
	{
		if (GetEffect() != nullptr)
		{
			trigger(!m_keyIn);
		}

		Interactable::Update(window);
	}

	void TriggerKeyHole::onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state)
	{
		Input::DisplayIconManager::ShowKeyInputDisplay(keyRef::PLAYER_INTERACT, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), state & InputManager::KEY_STATE_HELD ? 8 : 0);

		if (state & InputManager::KEY_STATE_PRESSED)
		{
			if (!m_keyIn)
			{
				//Make sure the player has a key.
				if (player->GetInventory()->GetHeldItem() != nullptr && player->GetInventory()->GetHeldItem()->GetType() == ITEM_TYPE::KEY)
				{
					InventoryItem* key = player->GetInventory()->GetHeldItem();

					player->GetInventory()->SetItem(player->GetInventory()->GetSelectedSlot() - 1, nullptr);
					delete key;	//No longer used by anything.
				}
				else {
					return;
				}
			}
			else {
				if (m_twoKeysIn) { return; }
				if (!m_keyRemovable) { return; }

				if (player->GetInventory()->GetHeldItem() != nullptr && player->GetInventory()->GetHeldItem()->GetType() == ITEM_TYPE::KEY)
				{
					InventoryItem* key = player->GetInventory()->GetHeldItem();

					player->GetInventory()->SetItem(player->GetInventory()->GetSelectedSlot() - 1, nullptr);
					delete key;	//No longer used by anything.

					m_sprite->SetSpriteId(m_sprite->GetSpriteId() + 1);
					m_twoKeysIn = true;
					MechanicalSaveManager::UpdateObject(this);
					return;
				}else if (player->GetInventory()->GetHeldItem() == nullptr)
				{
					player->GetInventory()->SetItem(player->GetInventory()->GetSelectedSlot() - 1, new InventoryItem(ITEM_TYPE::KEY));
				} else {
					FloorItem* floorItem = new FloorItem(m_position + m_scale / 2.f, new InventoryItem(ITEM_TYPE::KEY), 0.25f);
					Renderer::InstantiateObject(Renderer::InstantiateGameObject(floorItem, true, 2, false));
				}
			}

			m_keyIn = !m_keyIn;
			//Update the save data.
			MechanicalSaveManager::UpdateObject(this);

			if (m_keyIn)
			{
				m_sprite->SetSpriteId(m_sprite->GetSpriteId() + 1);
			}
			else {
				m_sprite->SetSpriteId(m_sprite->GetSpriteId() - 1);
			}
		}
	}

	MiscState::SaveParam TriggerKeyHole::Encode()
	{
		MiscState::SaveParam data;
		data.append(m_keyIn ? "t" : "f");
		data.append(m_twoKeysIn ? "t" : "f");

		return data;
	}

	void TriggerKeyHole::Decode(const SaveParam param)
	{
		//Check to make sure texture is right.
		bool keyShouldBeIn = param.starts_with("t");
		if (m_keyIn && !keyShouldBeIn)
		{
			m_keyIn = false;
			m_sprite->SetSpriteId(m_sprite->GetSpriteId() - 1);
		}
		else if (!m_keyIn && keyShouldBeIn)
		{
			m_keyIn = true;
			m_sprite->SetSpriteId(m_sprite->GetSpriteId() + 1);

			m_twoKeysIn = param.ends_with("t");
			if (m_twoKeysIn)
			{
				m_sprite->SetSpriteId(m_sprite->GetSpriteId() + 1);
			}
		}

	}
}
