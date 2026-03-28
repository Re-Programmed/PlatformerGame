#include "Foragable.h"

#include "../../../Resources/Save/SaveManager.h"
#include "../../InputDisplay/DisplayIconManager.h"

#include "../../../Utils/Time/GameTime.h"

#include "../../Items/FloorItem.h"

#include "../../../Objects/StateSaver.h"

#include "../../Cutscenes/DialogueManager.h"

namespace GAME_NAME::Objects::Environment
{
	double Forageable::m_forageTimer = 0.0;

	Forageable::Forageable(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, size_t saveID, Items::InventoryItem* drops, double forageLength, ITEM_TYPE requiredTool)
		: GameObjectState(saveID), Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, scale.X * 1.45f, position, scale, sprite), m_forageLength(forageLength), m_drop(drops), m_requiredTool(requiredTool)
	{
		LoadState();
	}

	void Forageable::LoadState()
	{
		std::string result("f");
		Resources::SaveManager::GetLevelString(result, m_objectSaveID);

		setForaged(result.starts_with("t"));
	}

	void Forageable::SaveState()
	{
		Resources::SaveManager::SaveLevelString(m_foraged ? ("t") : ("f"), m_objectSaveID);
	}

	void Forageable::Update(GLFWwindow* window)
	{
		Interactable::Update(window);
	}

	void Forageable::onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state)
	{
		if (!m_foraged)
		{
			Input::DisplayIconManager::ShowKeyInputDisplay(keyRef::PLAYER_INTERACT, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), (char)((m_forageTimer/m_forageLength) * 10.0));

			//Hold the forage button until complete, then set foraged and spawn drops.
			if (state & InputManager::KEY_STATE_HELD)
			{
				//Check if a specific tool is required.
				if (this->m_requiredTool != ITEM_TYPE::NULL_ITEM)
				{
					if (!(player->GetInventory()->GetHeldItem() != nullptr && player->GetInventory()->GetHeldItem()->GetType() == m_requiredTool))
					{
						if (Cutscenes::DialogueManager::INSTANCE->IsSequencePlaying()) { return; }

						std::string itemName = ITEMTYPE_GetItemTypeName(this->m_requiredTool);

						int rand = std::rand();
						int r = (4 * rand) / RAND_MAX;

						std::string message = "I must have left my " + itemName + " %item_" + std::to_string(this->m_requiredTool) + " at home!";

						switch (r)
						{
						case 0:
							message = "I could use a " + itemName + " %item_" + std::to_string(this->m_requiredTool) + " to get this.";
							break;
						case 1:
							message = "I could go for a " + itemName + " %item_" + std::to_string(this->m_requiredTool) + " right now.";
							break;
						case 2:						
							message = "Oh. I misplaced my " + itemName + " %item_" + std::to_string(this->m_requiredTool) + " .";
							break;
						}

						Cutscenes::DialogueManager::INSTANCE->PlayDialogueSequence(Cutscenes::DialogueSequence(1, Cutscenes::DialogueSequence::DialogueEvent(message, TestGame::ThePlayer.get())));

						return;
					}
				}

				m_forageTimer += Utils::Time::GameTime::DeltaTime::GetDeltaTime();

				if (m_forageTimer >= m_forageLength)
				{
					if (m_drop != nullptr)
					{
						Items::FloorItem* floorItem = new Items::FloorItem(m_position + m_scale / 2.f, m_drop, 0.5f);
						Renderer::InstantiateObject(Renderer::InstantiateGameObject(floorItem, true, 1, false));
					}

					m_forageTimer = 0.0;

					setForaged(true);

					if (!m_toBeSaved)
					{
						StateSaver::RegisterToBeSaved(this);
						SetToBeSaved(true);
					}

				}
			}
		}
	}

	void Forageable::setForaged(bool foraged)
	{
		if (m_foraged && !foraged)
		{
			//No longer foraged.
			this->m_sprite->SetSpriteId(m_sprite->GetSpriteId() - 1);
			m_foraged = false;
		}
		else if(!m_foraged && foraged){
			//Foraged.
			this->m_sprite->SetSpriteId(m_sprite->GetSpriteId() + 1);
			m_foraged = true;
		}
	}

}
