#pragma once

#include "../../../Objects/GUI/GUIButton.h"
#include "../ItemType.h"
#include "../InventoryItem.h"
#include "../Inventory.h"

namespace GAME_NAME::Items::Inventories
{
#define ITEM_SELECTION_BOX_BG_SPRITE SpriteBase(229)


using namespace GAME_NAME::Objects;
using namespace GAME_NAME::Objects::GUI;

	class ItemSelectionSlot
		: public GUIButton
	{
	public:
		/// <summary>
		/// Creates a slot with a menu that can be used to set the item inside.
		/// </summary>
		/// <param name="position"></param>
		/// <param name="scale"></param>
		/// <param name="bgTexture"></param>
		/// <param name="initialItem">What item starts in the slot.</param>
		/// <param name="canBeChanged">If this is false, the slot will not be editable by the user.</param>
		/// <param name="customOnClick">Called when the item of this slot is changed. If this slot cannot be modified, the function will be called when the slot is clicked.</param>
		ItemSelectionSlot(Vec2 position, Vec2 scale, GLuint bgTexture, InventoryItem* initialItem, bool canBeChanged = true, std::function<void(int)>* customOnClick = nullptr);

		~ItemSelectionSlot();

		inline InventoryItem* GetContainedItem()
		{
			return m_currentItem;
		}

		void CreateItemSelectionMenu(const int sourcesCount, ...);
		static void RemoveItemSelectionMenu();

		void SetContainedItem(InventoryItem* item);
		
		void Render(float zoom) override;

	protected:
		inline bool GetCanBeChanged()
		{
			return m_canBeChanged;
		}

		//Called when the item of this slot is changed. If this slot cannot be modified, the function will be called when the slot is clicked.
		std::function<void(int)>* m_customOnClick;

	private:
		bool m_canBeChanged;

		struct ItemSelectionMenu
		{
			StaticGUIElement* Backing = nullptr;
			std::vector<GUIButton*> Options;

			ItemSelectionSlot* CurrentBox = nullptr;

			std::vector<InventoryItem*> Items;
		};

		static ItemSelectionMenu m_currentItemMenu;

		static std::vector<ItemSelectionSlot*> m_selectionBoxes;

		InventoryItem* m_currentItem;

		static void onClick(int buttonID);
		static void onClickSlotBox(int buttonID);
	};
}