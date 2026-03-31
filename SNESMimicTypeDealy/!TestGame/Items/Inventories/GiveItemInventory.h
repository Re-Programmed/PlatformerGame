#pragma once
#include <functional>
#include "../InventoryItem.h"
#include "./ItemSelectionSlot.h"
#include "../../Objects/GUI/GUIItemSelectionBox.h"
#include "../../../Objects/GUI/GUIButton.h"

#include "../../../Objects/GUI/Text/TextRenderer.h"

namespace GAME_NAME::Items::Inventories
{
	class GiveItemInventory
	{
	public:
		/// <summary>
		/// Opening the inventory will create all the respective GUI and reset the callback for when an item is selected.
		/// </summary>
		static void Open();
		/// <summary>
		/// Closing the inventory will destroy all the created GUI and clear respective memory.
		/// </summary>
		static void Close();

		inline static bool IsOpen() { return m_giveItemMenu.InventorySelectionArea; }

		inline static void SetItemCallback(std::function<bool(ITEM_TYPE)> callback)
		{
			m_recieveItemCallback = callback;
		}

	private:
		static void itemSelectionUpdated(ITEM_TYPE item, int count);
		static void itemChosen(int buttonID);

		/// <summary>
		/// All the elements that represent the GUI graphics.
		/// </summary>
		typedef struct GiveItemMenu
		{
			GUI::StaticGUIElement* Backing = nullptr;

			GUI::GUIItemSelectionBox* InventorySelectionArea = nullptr;
			ItemSelectionSlot* SelectedSlot = nullptr;
			GUI::GUIButton* GiveButton = nullptr;

			GUI::Text::TextRenderer::ExpectedRenderedWord GiveText;
		};

		static GiveItemMenu m_giveItemMenu;

		/// <summary>
		/// Can be set to be called when the item changes.
		/// </summary>
		static std::function<bool(ITEM_TYPE)> m_recieveItemCallback;
	};
}