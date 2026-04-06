#include "Furniture.h"

#include <sstream>
#include "../Items/ItemType.h"

#include "../TestGame.h"
#include "../../Utils/CollisionDetection.h"
#include "../../Input/InputManager.h"
#include "../../Utils/Time/GameTime.h"

#include "../Level/Hub/HouseManager.h"

#include "../InputDisplay/DisplayIconManager.h"

#include "../Items/Crafting/CraftingMenuManager.h"
#include "../Level/Hub/HubLevelManager.h"

#define FURNITURE_TEXTURE_FLIP_SIT_OFFSET Vec2{ 2.f, 5.f }
#define FURNITURE_TEXTURE_NOT_FLIP_SIT_OFFSET Vec2{ -2.f, 5.f }

namespace GAME_NAME::Objects
{
	double Furniture::InteractionTimer = 0.0;


	const std::unordered_map<ITEM_TYPE, Furniture::FurnitureInteractions> Furniture::FurnitureAbilities = {
		{ ITEM_TYPE::SMALL_WOODEN_CHAIR, Furniture::FurnitureInteractions::Sit },
		{ ITEM_TYPE::FRIDGE, Furniture::FurnitureInteractions::Inventory },
		{ ITEM_TYPE::WOODEN_CRATE, Furniture::FurnitureInteractions::Inventory },
		{ ITEM_TYPE::WORKBENCH, Furniture::FurnitureInteractions::Workbench },
		{ ITEM_TYPE::WOODEN_BENCH, Furniture::FurnitureInteractions::Sit },
	};

	Furniture::FurnitureItemInfo Furniture::GetInfo(std::string furnitureData)
	{
		FurnitureItemInfo ret{ 0 };

		std::stringstream furnitureInfo(furnitureData);
		std::string data;

		int i = 0;
		while (std::getline(furnitureInfo, data, ','))
		{
			switch (i++)
			{
			case 0:
				ret.Scale.X = std::stoi(data);
				break;
			case 1:
				ret.Scale.Y = std::stoi(data);
				break;
			case 2:
				ret.InventorySize = std::stoi(data);
				break;
			case 3:
				ret.IsOutdoor = std::stoi(data) > 0;
				break;
			}
		}

		return ret;
	}

	//When it is used by a character... (not to be confused with the use method for the player...)
	bool Furniture::Use(Cutscenes::AnimatingCharacter* character)
	{
		if (m_inUse) { return false; }

		if (m_inUseByCharacter)
		{
			if (m_inUseByCharacter == character)
			{
				//Reset attributes. (No longer in use).
				m_inUseByCharacter->SetFrozen(false);
				//Make the character move next to the furniture.
				m_inUseByCharacter->SetTarget(m_position - Vec2{ 15.f, 0.f });
				m_collider->SetActive(true);

				m_inUseByCharacter = nullptr;
				return true;
			}

			return false;
		}

		m_inUseByCharacter = character;

		//Determine what to do upon interction based on this furniture's abilities.

		switch (FurnitureAbilities.at(m_item->GetType()))
		{
		case FurnitureInteractions::Sit:
			//The character is doing something else, don't let them sit.
			if (m_inUseByCharacter->IsFrozen()) { return false; }

			m_inUseByCharacter->SetFrozen(true, Objects::Player::Player::SITTING_FORWARD);
			m_inUseByCharacter->SetTextureFlipped(m_scale.X < 0);
			m_inUseByCharacter->SetPosition(m_position + m_scale / 2.f - m_inUseByCharacter->GetScale() / 2.f + ((m_scale.X < 0) ? FURNITURE_TEXTURE_FLIP_SIT_OFFSET : FURNITURE_TEXTURE_NOT_FLIP_SIT_OFFSET) - Vec2{ 0.f, 2.f });
			m_collider->SetActive(false);
			break;
		}

		return true;
	}

	void Furniture::RenderPreview(const Vec2& cameraPos, Items::ITEM_TYPE item, Vec2 position, bool flipped)
	{
		FurnitureItemInfo info = GetInfo(Items::ITEMTYPE_GetItemData(item).Attributes.at(Items::FURNITURE));

		Sprite* sp = Items::ITEMTYPE_GetItemTypeTexture(item);
		DynamicSprite sprite(sp->GetSpriteId());
		delete sp;

		Vec2& scale = info.Scale;
		scale = scale * Vec2{ flipped ? 1.f : -1.f, -1.f };

		const bool validPos = HouseManager::CheckValidPlaceLocation(position, scale);

		const Vec4 col = { 0.6f + (validPos ? ((float)InteractionTimer * 0.4f) : 0.4f), 0.6f + ((float)InteractionTimer * 0.4f), 0.6f + (validPos ? 0.4f : ((float)InteractionTimer * 0.4f)), 0.6f + ((float)InteractionTimer * 0.4f)};

		const Vec4 Furniture_previewColors[4] = { col, col, col, col };

		sprite.UpdateTextureColor(Furniture_previewColors);

		sprite.Render(cameraPos, position + Vec2{ 0.f, -scale.Y/2.f }, scale);
	}

	void Furniture::Update(GLFWwindow* window)
	{
		//If the furniture is outdoor and we are not outside, do nothing.
		//The only reason this furniture is kept is to save to the save file.
		if (m_isOutdoor == HubLevelManager::GetInHouse()) { return; }

		//No collision for outdoor furniture.
		if (this->m_isOutdoor) { m_collider->SetActive(false); }

		if (m_inUse)
		{
			use();	
		}

		if (FurnitureAbilities.at(m_item->GetType()) != FurnitureInteractions::None && FurnitureAbilities.at(m_item->GetType()) != FurnitureInteractions::Inventory)
		{
			if (Vec2::Distance(TestGame::ThePlayer->GetPosition() + TestGame::ThePlayer->GetScale() / 2.f, m_position + m_scale / 2.f) < 1.13f * ((m_scale.X > m_scale.Y) ? m_scale.X : m_scale.Y))
			{
				Input::DisplayIconManager::ShowKeyInputDisplay(keyRef::PLAYER_INTERACT, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), 0);

				if (InputManager::GetKeyUpDown(keyRef::PLAYER_INTERACT) & InputManager::KEY_STATE_PRESSED)
				{
					//To prevent weird softlocking when you use a chair and other stuff at the same time.
					if (!TestGame::ThePlayer->GetFrozen())
					{
						use();
						
						//If it is a workbench, only use it once.
						if (FurnitureAbilities.at(m_item->GetType()) != FurnitureInteractions::Workbench)
						{
							m_inUse = true;
						}
					}
				}
			}
		}

		m_hovered = false;

		Vec2 mousePos = InputManager::GetMouseWorldPosition(TestGame::INSTANCE->GetCamera());

		Vec2 checkPos = m_position;
		Vec2 checkScale = m_scale;
		if (m_flipped)
		{
			checkPos += Vec2{ m_scale.X, 0.f };
			checkScale = Vec2{ -m_scale.X, m_scale.Y };
		}

		if (CollisionDetection::PointWithinBoxBL(mousePos, checkPos, checkScale))
		{
			m_hovered = true;

			if (InputManager::GetMouseButton(0))
			{
				//No removing furniture with items in them.
				if (m_inventory != nullptr)
				{
					if (!m_inventory->IsEmpty())
					{
						m_interactionRemovalTimer = 0.0;
						LayerFlipObject::Update(window);
						return;
					}
				}

				m_interactionRemovalTimer += Utils::Time::GameTime::DeltaTime::GetDeltaTime();

				if (m_interactionRemovalTimer > 1.0)
				{
					InventoryItem* item = new InventoryItem(m_item->GetType());

					if (TestGame::ThePlayer->GetInventory()->AddItem(item) == -1)
					{
						if (TestGame::ThePlayer->GetBackpack()->AddItem(item) == -1)
						{
							delete item;
							return;
						}
					}
					
					//Make the character stop using this if a character is using it.
					if (m_inUseByCharacter)
					{
						this->Use(m_inUseByCharacter);
					}

					HouseManager::RemoveFurniture(this);
					Renderer::DestroyActiveObjectImmediate(this);
					delete this;
					return;
				}
				return;
			}
		}


		LayerFlipObject::Update(window);

		m_interactionRemovalTimer = 0.0;
	}

	void Furniture::Render(const Vec2& cameraPos)
	{
		//If the furniture is outdoor and we are not outside, do nothing.
		//The only reason this furniture is kept is to save to the save file.
		if (m_isOutdoor == HubLevelManager::GetInHouse()) { return; }


		if (m_hovered)
		{
			//No removing furniture with items in them.
			if (m_inventory != nullptr)
			{
				if (!m_inventory->IsEmpty())
				{
					m_interactionRemovalTimer = 0;
					LayerFlipObject::Render(cameraPos);
					return;
				}
			}

			DynamicSprite sprite(m_sprite->GetSpriteId());

			const Vec4 col = { 1.f, 0.6f - ((float)m_interactionRemovalTimer * 0.6f), 0.6f - ((float)m_interactionRemovalTimer * 0.6f), 1.f - (float)m_interactionRemovalTimer };
			const Vec4 colors[4] = { col, col, col, col };

			sprite.UpdateTextureColor(colors);

			sprite.Render(cameraPos, m_position + m_scale, m_scale * Vec2{ -1.f }, m_rotation);
			return;
		}

		LayerFlipObject::Render(cameraPos);
	}

	//SAVEDATA: (xPos),(yPos),(itemType)

	MiscState::SaveParam Furniture::Encode()
	{
		std::string ret = std::to_string(m_position.X).append(",")
			.append(std::to_string(m_position.Y)).append(",")
			.append(std::to_string(m_item->GetType())).append(",")
			.append(m_flipped ? "t" : "f").append(",")
			.append(m_isOutdoor ? "1" : "0");


		if (m_inventory != nullptr)
		{
			ret.append(",");

			for (int i = 0; i < m_inventory->GetSize(); i++)
			{
				ret.append(m_inventory->GetItem(i).ri_IsNull ? "NULL" : m_inventory->GetItem(i).ri_Item->Encode()).append(";");
			}

			ret.erase(ret.end() - 1);
		}

		return ret;
	}

	void Furniture::Decode(const SaveParam param)
	{
		std::stringstream ss(param);

		std::string line;
		uint8_t i = 0;
		while (std::getline(ss, line, ','))
		{
			switch (i)
			{
			case 0:
				m_position.X = std::stof(line);
				break;
			case 1:
				m_position.Y = std::stof(line);
				break;
			case 2:
			{
				m_item = new Items::InventoryItem(static_cast<Items::ITEM_TYPE>(std::stoi(line)));
				//Info on the scale of the furniture is stored in the item data.
				std::string scaleInfo = Items::ITEMTYPE_GetItemData(m_item->GetType()).Attributes.at(Items::FURNITURE);

				m_scale.X = std::stof(scaleInfo.substr(0, scaleInfo.find_first_of(',')));
				m_scale.Y = std::stof(scaleInfo.substr(scaleInfo.find_first_of(',') + 1));

				SetSprite(Items::ITEMTYPE_GetItemTypeTexture(m_item->GetType()));
				break;
			}
			case 3:
				m_flipped = line == "t";
				if (m_flipped)
				{
					m_scale.X = -m_scale.X;
				}
				break;
			case 4:
				m_isOutdoor = std::stoi(line) > 0;
				if (m_isOutdoor)
				{
					Cutscenes::CharacterNodeManager::RegisterNode(this);
				}
				break;
			case 5:

				if (FurnitureAbilities.at(m_item->GetType()) == FurnitureInteractions::Inventory)
				{
					std::string attr = ITEMTYPE_GetItemData(m_item->GetType()).Attributes.at(FURNITURE);
					int invSize = std::stoi(attr.substr(attr.find_last_of(',') + 1));

					m_inventory = new Items::Inventories::InventoryContainer(ITEMTYPE_GetItemTypeName(m_item->GetType()), invSize, m_position - Vec2{ m_scale.X < 0.f ? 4.f - m_scale.X : 4.f, 4.f }, (m_scale.X < 0.f ? Vec2{ -m_scale.X, m_scale.Y } : m_scale) + Vec2{ 8.f, 8.f }, nullptr, 0, 0.f, false);


					Renderer::InstantiateObject(Renderer::InstantiateGameObject(m_inventory, false, 1, false));

					std::stringstream itemStream(line);
					std::string itemCode;

					while (std::getline(itemStream, itemCode, ';'))
					{
						if (itemCode == "NULL") { continue; }

						m_inventory->AddItem(InventoryItem::DecodeItemString(itemCode));
					}
				}

				break;
			}
			
			i++;
		}

		m_collider->SetPosition(m_position);

		if (m_scale.X > 0)
		{
			m_collider->SetScale({ m_scale.X, 10.f });
		}
		else {
			m_collider->Translate(Vec2{ m_scale.X, 0.f });
			m_collider->SetScale({ -m_scale.X, 10.f });
		}
		
	}

	//Will be called with m_inUse = false if it is the first time the Furniture is used.
	//Will be called with m_inUse = true if it is updating.
	void Furniture::use()
	{
		if (m_inUseByCharacter) { return; }

		switch (FurnitureAbilities.at(m_item->GetType()))
		{
		case FurnitureInteractions::Sit:
			useSit();
			break;
		case FurnitureInteractions::Workbench:
			if (m_inUse) { return; }	//Not the first use event.
			if (!Crafting::CraftingMenuManager::CloseCraftingMenu())
			{
				Crafting::CraftingMenuManager::OpenCraftingMenu();
			}

			m_inUse = false;
			break;
		}
	}


	void Furniture::useSit()
	{

		if (!m_inUse)
		{
			m_collider->SetActive(false);
			TestGame::ThePlayer->SetFrozen(true, Player::Player::SITTING_FORWARD);
			TestGame::ThePlayer->SetTextureFlipped(m_scale.X > 0);
		}

		if (TestGame::ThePlayer->GetLookDirection() == Player::Player::SITTING_FORWARD && TestGame::ThePlayer->GetFrozen())
		{
			TestGame::ThePlayer->SetPosition(m_position + m_scale / 2.f - TestGame::ThePlayer->GetScale() / 2.f + ((m_scale.X < 0) ? FURNITURE_TEXTURE_FLIP_SIT_OFFSET : FURNITURE_TEXTURE_NOT_FLIP_SIT_OFFSET));
		}
		else {
			m_inUse = false;
			m_collider->SetActive(true);
		}
	}
}

