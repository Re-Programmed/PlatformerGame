#pragma once
#include "../../Items/Inventory.h"
#include "../../../Objects/GUI/StaticGUIElement.h"
#include "../../Items/InventoryItem.h"
#include "../../../Objects/MiscStateGroup.h"

namespace GAME_NAME
{
	typedef uint8_t InventorySlot;
	class ScreenInventory
		: public GAME_NAME::Items::Inventory, public MiscStateGroup
	{
	public:
		ScreenInventory();

		void Update();

		int AddItem(Items::InventoryItem* item, bool ignoreSave = false);

		int RemoveItemType(Items::ITEM_TYPE type, unsigned int count);

		bool SetItem(uint8_t slot, Items::InventoryItem* item);

		inline Items::InventoryItem* const GetHeldItem()
		{
			uint8_t& slot = m_saveMetadata.SelectedSlot;
			if (m_items.size() <= slot - 1)
			{
				return nullptr;
			}

			return m_items[slot - 1];
		}

		inline InventorySlot GetSelectedSlot()
		{
			return m_saveMetadata.SelectedSlot;
		}

		/// <summary>
		/// Hides the main display in the lower left for the players items.
		/// </summary>
		void HidePlayerSlots();
		/// <summary>
		/// Shows the main display in the lower left for the players items.
		/// </summary>
		void ShowPlayerSlots();

		/// <summary>
		/// Selects the given slot.
		/// </summary>
		/// <param name="slot"></param>
		/// <param name="updatePlayerDisplay"></param>
		void SelectSlot(InventorySlot slot, bool updatePlayerDisplay = true);
	private:
		///Used to store the save data for the ScreenInventory such as the currently selected item.
		struct ScreenInventoryMetadata
			: public MiscState
		{
			InventorySlot SelectedSlot = 0;

			MiscState::SaveParam Encode() final override
			{
				return std::to_string(SelectedSlot);
			}

			void Decode(const MiscState::SaveParam params) final override
			{
				SelectedSlot = std::stoi(params);
			}
		};
		ScreenInventoryMetadata m_saveMetadata;

		GUI::StaticGUIElement* m_slots[3];
		/// <summary>
		/// Contains the sprite currently rendered for each item in its corresponding slot, nullptr for no item.
		/// </summary>
		GUI::StaticGUIElement* m_itemSprites[3];

		void updateSave();
		
		/// <summary>
		/// Instantiates the item display for the given slot and item.
		/// </summary>
		void createItemSpriteDisplay(uint8_t slot, Items::InventoryItem& item);
	};
}