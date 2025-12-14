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

		GUIItemSelectionBox(Vec2 position, float cutoffHeight, ItemSelection items[], size_t itemCount, std::function<void(Items::ITEM_TYPE, int)>* m_selectedCallback);

		~GUIItemSelectionBox();

		void Render(float zoom) override;

		void AddItemSelection(ItemSelection item);

		static void OptionClicked(int buttonID);

		//Returns the item selection based on the given button id.
		ItemSelection GetItemSelected(int buttonID, bool& isValid);

		

	private:
		inline void selectedCallback(Items::ITEM_TYPE type, int count)
		{
			if (m_selectedCallback != nullptr)
			{
				auto func = *m_selectedCallback.get();
				func(type, count);
			}
		}

		std::unique_ptr<std::function<void(Items::ITEM_TYPE, int)>> m_selectedCallback;

		std::vector <ItemSelection> m_itemContents;
	};
}