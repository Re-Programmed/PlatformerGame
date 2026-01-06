#pragma once

#include "../../../Objects/GUI/GUIScrollArea.h"
#include "../../Items/ItemType.h"

namespace GAME_NAME::Objects::GUI
{
	class GUIItemSelectionBox
		: public GUIScrollArea
	{
	public:
		typedef struct ItemSelection
		{
			Items::ITEM_TYPE Type;
			uint8_t Count;

			int ButtonID = 0;
		};

		GUIItemSelectionBox(Vec2 position, float cutoffHeight, ItemSelection items[], size_t itemCount, std::function<void(Items::ITEM_TYPE, int)>* m_selectedCallback, bool showSelected = true);

		~GUIItemSelectionBox();

		void Render(float zoom) override;

		void AddItemSelection(ItemSelection item);

		static void OptionClicked(int buttonID);

		//Returns the item selection based on the given button id.
		ItemSelection GetItemSelected(int buttonID, bool& isValid);

		/// <summary>
		/// Sets the currently selection button. Use -1 for no selection.
		/// </summary>
		/// <param name="buttonID"></param>
		void SetCurrentSelection(int buttonID);

		ItemSelection GetCurrentSelection(bool& isSelected);

		inline const std::vector<ItemSelection> GetContents()
		{
			return m_itemContents;
		}

		bool ShowsSelected() { return m_showSelected; }

		/// <summary>
		/// Removes an item from a selection box based on some given condition.
		/// </summary>
		/// <param name="box"></param>
		/// <param name="condition"></param>
		/// <returns></returns>
		static bool RemoveItemFromSelectionBox(GUIItemSelectionBox*& box, std::function<bool(GUIItemSelectionBox::ItemSelection)> condition, std::function<void(GAME_NAME::Items::ITEM_TYPE, int)>* selectCallback, int layer = 2);


	private:
		bool m_showSelected = true;

		int m_currentSelection = -1;

		void selectedCallback(Items::ITEM_TYPE type, int count, const int& buttonID);

		std::unique_ptr<std::function<void(Items::ITEM_TYPE, int)>> m_selectedCallback;

		std::vector<ItemSelection> m_itemContents;
	};
}