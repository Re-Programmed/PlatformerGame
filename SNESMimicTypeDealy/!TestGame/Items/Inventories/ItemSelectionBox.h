#pragma once

#include "../../../Objects/GUI/GUIButton.h"
#include "../ItemType.h"
#include "../InventoryItem.h"
#include "../Inventory.h"

namespace GAME_NAME::Items::Inventories
{
using namespace GAME_NAME::Objects;
using namespace GAME_NAME::Objects::GUI;

	class ItemSelectionBox
		: public GUIButton
	{
	public:
		ItemSelectionBox(Vec2 position, Vec2 scale, GLuint bgTexture, InventoryItem* initialItem);

		~ItemSelectionBox();

		inline InventoryItem* GetContainedItem()
		{
			return m_currentItem;
		}

		void CreateItemSelectionMenu(const unsigned int&& sourcesCount, ...);

		void SetContainedItem(InventoryItem* item);

	private:
		struct ItemSelectionMenu
		{
			StaticGUIElement* Backing;
			std::vector<GUIButton*> Options;

			ItemSelectionBox* CurrentBox;

			std::vector<InventoryItem*> Items;
		};

		static ItemSelectionMenu m_currentItemMenu;

		static std::vector<std::unique_ptr<ItemSelectionBox>> m_selectionBoxes;

		InventoryItem* m_currentItem;

		static void onClick(int buttonID);
		static void onClickSlotBox(int buttonID);
	};
}