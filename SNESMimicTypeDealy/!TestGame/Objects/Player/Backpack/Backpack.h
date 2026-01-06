#pragma once


#include "../../../Items/Inventory.h"
#include "../../../../Objects/GUI/GUIButton.h"

#include "../../../../Objects/MiscStateGroup.h"

#define BACKPACK_NUM_EQUIPMENT_SLOTS 3

using namespace GAME_NAME::Items;
using namespace GAME_NAME::Objects::GUI;

namespace GAME_NAME::Objects::Player
{
	/*
		Backpack Inventory: (m_items)
			[0-BACKPACK_NUM_EQUIPMENT_SLOTS] Euipment Slots
			[BACKPACK_NUM_EQUIPMENT_SLOTS - m_size] General Items
	*/

	/// <summary>
	/// A backpack is a craftable item that expands the player's inventory and provides equipment slots for equipables.
	/// The default backpack has 0 size and grows as the player progresses.
	/// </summary>
	class Backpack
		: public Inventory, public MiscStateGroup
	{
	public:
		/// <summary>
		/// Create a backpack with given size (or 0 size for just equipment slots).
		/// </summary>
		/// <param name="size">[uint8_t] - Backpack storage size.</param>
		Backpack(uint8_t size);

		/// <summary>
		/// Renders the backpack inventory to the screen.
		/// </summary>
		bool Open(bool ignoreAnimation = false);
		/// <summary>
		/// Closes the backpack. (if it is currently open)
		/// </summary>
		bool Close(bool ignoreAnimation = false);

		/// <summary>
		/// Should be called each frame to update the rendering of items and cursor display (no need to call UpdateHeldItemDisplay).
		/// </summary>
		void Render();

		/// <summary>
		/// Called each update to update the position of the held item graphic.
		/// </summary>
		void UpdateCursorItemDisplay();
			
		/// <summary>
		/// Returns true if the backpack GUI is currently rendered.
		/// </summary>
		/// <returns>[bool] - True if the backpack GUI is currently rendered.</returns>
		inline bool GetIsOpen() { return m_isOpen; }

		/// <summary>
		/// The backpack that is currently opened.
		/// </summary>
		static Backpack* CurrentOpenBackpack;

		/// <summary>
		/// Move constructor that moves all items from a backpack to a new one, clearing the existing backpack.  (TODO: FIX THIS CONSTRUCTOR)
		/// </summary>
		/// <param name="other"></param>
		/// <param name="newSize"></param>
		Backpack(Backpack& other, uint8_t newSize)
			: m_isOpen(other.m_isOpen), Inventory("Backpack", newSize), MiscStateGroup("bk"), m_equipmentDisplayItems{nullptr, nullptr, nullptr}, m_equipmentSlots{nullptr, nullptr, nullptr}
		{
			//TODO: COPY THE INVENTORY OF OTHER TO THIS BAG.
		}

		int AddItem(InventoryItem* item);
	private:
		/// <summary>
		/// Links each button ID to an inventory ID.
		/// </summary>
		static std::vector<uint32_t> m_buttonIDSync;

		/// <summary>
		/// True if the backpack is open.
		/// </summary>
		bool m_isOpen = false;

		/// <summary>
		/// All the backings and display features of the backpack that are rendered.
		/// </summary>
		std::vector<StaticGUIElement*> m_displayFeatures;

		/// <summary>
		/// When displaying the backpack, these slots point to the equipment slots that can be clicked on.
		/// </summary>
		GUIButton* m_equipmentSlots[BACKPACK_NUM_EQUIPMENT_SLOTS];

		/// <summary>
		/// The item currently on the cursor.
		/// </summary>
		static InventoryItem* m_cursorItem SERIALIZED;
		
		/// <summary>
		/// Attempts to place an item on the cursor. If an item exists on it or an empty item is given, returns nullptr.
		/// </summary>
		/// <param name="item">[InventoryItem*] - The item to attempt to place.</param>
		static StaticGUIElement* setCursorItem(InventoryItem* item);
		
		static StaticGUIElement* m_cursorItemDisplay;

		/// <summary>
		/// The general item slots currently rendered by the backpack.
		/// </summary>
		std::vector<GUIButton*> m_generalSlots;

		std::vector<StaticGUIElement*> m_generalDisplayItems;
		StaticGUIElement* m_equipmentDisplayItems[BACKPACK_NUM_EQUIPMENT_SLOTS];

		/// <summary>
		/// Renders the equipment slots on the right side of the screen.
		/// </summary>
		void loadEquipmentSlots(const bool& ignoreAnimation);

		/// <summary>
		/// Used to ensure the user can only click on a slot a certain number of times every second.
		/// </summary>
		static double m_clickDelay;

		/// <summary>
		/// Callback for clicking an equipment slot.
		/// </summary>
		/// <param name="id">[int] - Button ID.</param>
		static void clickEquipmentSlot(int id);
		static void clickGeneralItemSlot(int id);

		static void clickPlayerSlot(int id);
		
		/// <summary>
		/// Utility method for updating slot when clicked on.
		/// Index is index of the item slot clicked on.
		/// Required action is the abilities required by the held item to enter a slot if clicked.
		/// </summary>
		/// <param name="index"></param>
		static void updateSlotItem(const int& index, int requiredToolAction = -1);

		static StaticGUIElement* m_playerSlots[3];
		static void createPlayerSlots();
		static void removePlayerSlots();

		/// <summary>
		/// True if the fade of the slots is currently occuring, should prevent interactions with the bag.
		/// </summary>
		static bool m_animating;
		static std::mutex m_animatingMutex;

	};
}
