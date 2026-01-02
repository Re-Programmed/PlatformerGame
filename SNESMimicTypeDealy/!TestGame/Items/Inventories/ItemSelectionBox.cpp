#include "ItemSelectionBox.h"

#include <cstdarg>

#define ITEM_SELECTION_BOX_SELECTION_SCALE Vec2{ 100.f, 80.f }

namespace GAME_NAME::Items::Inventories
{
	std::vector<std::unique_ptr<ItemSelectionBox>> ItemSelectionBox::m_selectionBoxes;

	ItemSelectionBox::ItemSelectionBox(Vec2 position, Vec2 scale, GLuint bgTexture, InventoryItem* initialItem)
		: GUIButton(position, scale, bgTexture, new std::function<void(int)>(onClick)), m_currentItem(initialItem)
	{
		m_selectionBoxes.push_back(std::unique_ptr<ItemSelectionBox>(this));
	}

	ItemSelectionBox::~ItemSelectionBox()
	{
		for(unsigned int i = 0; i < m_selectionBoxes.size(); i++)
		{
			if (m_selectionBoxes[i].get() == this)
			{
				m_selectionBoxes.erase(m_selectionBoxes.begin() + i);

				GUIButton::~GUIButton();
				return;
			}
		}

		GUIButton::~GUIButton();
	}

	void ItemSelectionBox::CreateItemSelectionMenu(const unsigned int&& sourcesCount, ...)
	{
		va_list args;
		va_start(args, sourcesCount);
		//Box already open.
		if (m_currentItemMenu.CurrentBox != nullptr)
		{
			return;
		}

		m_currentItemMenu.CurrentBox = this;
		
		Sprite* sp = Renderer::GetSprite(SpriteBase(-1));
		StaticGUIElement* backing = new StaticGUIElement(this->GetPosition() + Vec2{ this->GetScale().X, -ITEM_SELECTION_BOX_SELECTION_SCALE.Y }, ITEM_SELECTION_BOX_SELECTION_SCALE, sp->GetSpriteId());
		Renderer::LoadGUIElement(backing, 3);
		m_currentItemMenu.Backing = backing;
		delete sp;

		std::vector<InventoryItem*> items;

		uint8_t currentButton = 0;

		for (int i = 0; i < sourcesCount; i++)
		{
			Inventory* inv = va_arg(args, Inventory*);

			for (int s = 0; s < inv->GetSize(); s++)
			{
				Inventory::ReturnItem item = inv->GetItem(s);
				if (!item.ri_IsNull)
				{
					items.push_back(item.ri_Item);

					GUIButton* selectButton = new GUIButton(backing->GetPosition() + Vec2{ 100 * (currentButton%10), })
				}
			}
		}

		m_currentItemMenu.Items = items;
	}

	void ItemSelectionBox::SetContainedItem(InventoryItem* item)
	{
		m_currentItem = item;
	}

	void ItemSelectionBox::onClick(int buttonID)
	{
		for (unsigned int i = 0; i < m_selectionBoxes.size(); i++)
		{
			if (m_selectionBoxes[i]->GetButtonId() == buttonID)
			{
				m_selectionBoxes[i]->CreateItemSelectionMenu();
				return;
			}
		}
	}

	void ItemSelectionBox::onClickSlotBox(int buttonID)
	{
	}

}
