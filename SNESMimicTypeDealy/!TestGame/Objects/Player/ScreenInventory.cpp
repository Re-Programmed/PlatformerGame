#include "ScreenInventory.h"
#include "../../../Input/InputManager.h"
#include "../../TestGame.h"

#include "../../Items/Types/Tool.h"

#include "../../../CustomCompileOptions.h"

#define INVENTORY_SLOT_PADDING 5.f


namespace GAME_NAME
{

	ScreenInventory::ScreenInventory()
		: Inventory("Screen Inventory", 3), MiscStateGroup("si"), m_itemSprites{ nullptr, nullptr, nullptr }
	{
		m_slots[0] = new GUI::StaticGUIElement(Vec2(4, 20), Vec2(16, 16), Renderer::GetSprite(INVENTORY_SELECTED_SLOT_SPRITE)->GetSpriteId());
		m_slots[1] = new GUI::StaticGUIElement(Vec2(24, 20), Vec2(16, 16), Renderer::GetSprite(INVENTORY_UNSELECTED_SLOT_SPRITE)->GetSpriteId());
		m_slots[2] = new GUI::StaticGUIElement(Vec2(44, 20), Vec2(16, 16), Renderer::GetSprite(INVENTORY_UNSELECTED_SLOT_SPRITE)->GetSpriteId());

		ShowPlayerSlots();

		std::shared_ptr<std::vector<std::string>> data = getStates();
		
		if (data->size() > 0)
		{
			//Metadata is stored as first data string.
			m_saveMetadata.Decode(data->at(0));
			SelectSlot(m_saveMetadata.SelectedSlot, false);
			//Remove metadata for inventory, all other data saved represents items.
			data->erase(data->begin());

			using namespace Items;

			//Iterate over all save states, loading the corresponding item.
			for (int i = 0; i < data->size(); i++)
			{
				InventoryItem* ii = InventoryItem::DecodeItemString(data->at(i));
				if (ii != nullptr) { AddItem(ii, false); }
			}
		}
	}

	void ScreenInventory::Update()
	{
		{
			if (InputManager::GetKeyUpDown(PLAYER_SCREEN_INVENTORY_SLOT_1) & InputManager::KEY_STATE_PRESSED)
			{
				SelectSlot(1);
			}

			if (InputManager::GetKeyUpDown(PLAYER_SCREEN_INVENTORY_SLOT_2) & InputManager::KEY_STATE_PRESSED)
			{
				SelectSlot(2);
			}

			if (InputManager::GetKeyUpDown(PLAYER_SCREEN_INVENTORY_SLOT_3) & InputManager::KEY_STATE_PRESSED)
			{
				SelectSlot(3);
			}
		}
	}

	int ScreenInventory::AddItem(Items::InventoryItem* item, bool ignoreSave)
	{
		int s = Inventory::AddItem(item);

		if (s == -1)
		{
			return -1;
		}

		if (item != nullptr)
		{
			createItemSpriteDisplay(s, *item);
		}

		if(!ignoreSave) { updateSave(); }
		
		return s;
	}

	bool ScreenInventory::SetItem(uint8_t slot, Items::InventoryItem* item)
	{
		//Attempt to call base function, returning if failure occurs.
		if (!Inventory::SetItem(slot, item)) { return false; }

		//The item was removed from the inventory, clear the graphic.
		if (item == nullptr && m_itemSprites[slot] != nullptr)
		{
			Renderer::UnloadGUIElement(m_itemSprites[slot], 1);
			m_itemSprites[slot] = nullptr;
		}
		else if(item != nullptr)
		{
			//Remove the existing item graphic and data.
			if (!GetItem(slot).ri_IsNull)
			{
				Renderer::UnloadGUIElement(m_itemSprites[slot], 1);
				m_itemSprites[slot] = nullptr;
			}

			//Add the item display.
			createItemSpriteDisplay(slot, *item);
		}

		//Update save metadata.
		updateSave();

		return true;
	}

	void ScreenInventory::HidePlayerSlots()
	{
		Renderer::UnloadGUIElement(m_slots[0]);
		Renderer::UnloadGUIElement(m_slots[1]);
		Renderer::UnloadGUIElement(m_slots[2]);
	}

	void ScreenInventory::ShowPlayerSlots()
	{
		Renderer::LoadGUIElement(m_slots[0]);
		Renderer::LoadGUIElement(m_slots[1]);
		Renderer::LoadGUIElement(m_slots[2]);
	}

	void ScreenInventory::SelectSlot(InventorySlot slot, bool updatePlayerDisplay)
	{
		//Update save for selected slot.
		m_saveMetadata.SelectedSlot = slot;
		updateSave();

		slot--;

		for (InventorySlot i = 0; i < 3; i++)
		{
			if (slot == i)
			{
				m_slots[i]->SetSprite(Renderer::GetSprite(INVENTORY_SELECTED_SLOT_SPRITE));

				if (!updatePlayerDisplay) { continue; }

				if (m_items.size() > i && m_items[i] != nullptr)
				{
					TestGame::ThePlayer->SetHeldItem(m_items[i]);
				}
				else {
					TestGame::ThePlayer->RemoveHeldItem();
				}

				continue;
			}

			m_slots[i]->SetSprite(Renderer::GetSprite(INVENTORY_UNSELECTED_SLOT_SPRITE));
		}
	}

	void ScreenInventory::updateSave()
	{
		//Remove all save data.
		clearStates();
		//Assign the metadata.
		ScreenInventory::ScreenInventoryMetadata& metadata = m_saveMetadata;
		assignState(&metadata);
		//Add save data for all items.
		for (Items::InventoryItem* item : m_items)
		{
			if (item == nullptr)
			{
				MiscState* nullItem = new Items::InventoryItem(Items::NULL_ITEM);
				assignState(nullItem);
				continue;
			}

			assignState(item);
		}
	}

	void ScreenInventory::createItemSpriteDisplay(uint8_t slot, Items::InventoryItem& item)
	{
		Sprite* const sprite = Items::ITEMTYPE_GetItemTypeTexture(item.GetType());
		GUI::StaticGUIElement* itemDisplay = new GUI::StaticGUIElement(m_slots[slot]->GetPosition() + (INVENTORY_SLOT_PADDING / 2.f), m_slots[slot]->GetScale() - Vec2(INVENTORY_SLOT_PADDING), sprite->GetSpriteId());
		delete sprite;

		Renderer::LoadGUIElement(itemDisplay, 1);

		m_itemSprites[slot] = itemDisplay;
	}
}
