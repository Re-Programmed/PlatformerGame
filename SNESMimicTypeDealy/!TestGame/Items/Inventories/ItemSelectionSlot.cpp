#include "ItemSelectionSlot.h"

#include <cstdarg>
#include "../../TestGame.h"
#include "../../../CustomCompileOptions.h"

#include "../../../Objects/GUI/GUIManager.h"

#define ITEM_SELECTION_BOX_SELECTION_SCALE Vec2{ 100.f, 80.f }

namespace GAME_NAME::Items::Inventories
{
	std::vector<ItemSelectionSlot*> ItemSelectionSlot::m_selectionBoxes;
	ItemSelectionSlot::ItemSelectionMenu ItemSelectionSlot::m_currentItemMenu;


	ItemSelectionSlot::ItemSelectionSlot(Vec2 position, Vec2 scale, GLuint bgTexture, InventoryItem* initialItem, bool canBeChanged, std::function<void(int)>* customOnClick)
		: GUIButton(position, scale, bgTexture, new std::function<void(int)>(onClick)), m_canBeChanged(canBeChanged), m_customOnClick(customOnClick)
	{
		this->SetContainedItem(initialItem);

		m_selectionBoxes.push_back(this);
	}

	ItemSelectionSlot::~ItemSelectionSlot()
	{
		for(unsigned int i = 0; i < m_selectionBoxes.size(); i++)
		{
			if (m_selectionBoxes[i] == this)
			{
				m_selectionBoxes.erase(m_selectionBoxes.begin() + i);

				GUIButton::~GUIButton();
				return;
			}
		}

		GUIButton::~GUIButton();
	}

	void ItemSelectionSlot::CreateItemSelectionMenu(const int sourcesCount, ...)
	{
		if (sourcesCount == 0) { return; }

		va_list args;
		va_start(args, sourcesCount);

		//Box already open, close existing.
		if (m_currentItemMenu.CurrentBox != nullptr)
		{
			RemoveItemSelectionMenu();
		}

		m_currentItemMenu.CurrentBox = this;
		
		Sprite* sp = Renderer::GetSprite(SpriteBase(-1));
		StaticGUIElement* backing = new StaticGUIElement(this->GetPosition() + Vec2{ this->GetScale().X, -ITEM_SELECTION_BOX_SELECTION_SCALE.Y }, ITEM_SELECTION_BOX_SELECTION_SCALE, sp->GetSpriteId());
		delete sp;
		m_currentItemMenu.Backing = backing;
		Renderer::LoadGUIElement(backing, 2);

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

					Sprite* sp = ITEMTYPE_GetItemTypeTexture(item.ri_Item->GetType());
					GUIButton* selectButton = new GUIButton(backing->GetPosition() + Vec2{ 0.f, backing->GetScale().Y - 10.f } + Vec2{10.f * (currentButton % 10), -10.f * (currentButton / 10)}, Vec2{10.f}, sp->GetSpriteId(), new std::function<void(int)>(onClickSlotBox));
					delete sp;
					GUIManager::RegisterButton(selectButton);
					Renderer::LoadGUIElement(selectButton, 2);

					m_currentItemMenu.Options.push_back(selectButton);

					currentButton++;
				}
			}
		}

		//No items were found, no menu to make.
		if (m_currentItemMenu.Options.size() == 0)
		{
			m_currentItemMenu.CurrentBox = nullptr;

			Renderer::UnloadGUIElement(backing, 2);
			delete backing; backing = nullptr;
			return; 
		}

		backing->SetScale(Vec2{ currentButton >= 9 ? 100.f : 10.f * (currentButton % 10), 10.f * (currentButton / 10 + 1)});
		backing->SetPosition(Vec2{ backing->GetPosition().X, m_currentItemMenu.Options[m_currentItemMenu.Options.size() - 1]->GetPosition().Y });

		m_currentItemMenu.Items = items;
	}

	void ItemSelectionSlot::RemoveItemSelectionMenu()
	{
		if (m_currentItemMenu.CurrentBox == nullptr || m_currentItemMenu.Backing == nullptr)
		{
			return;
		}

		m_currentItemMenu.CurrentBox = nullptr;

		Renderer::UnloadGUIElement(m_currentItemMenu.Backing, 2);
 		delete m_currentItemMenu.Backing;

		m_currentItemMenu.Items.clear();

		for (GUIButton* button : m_currentItemMenu.Options)
		{
			GUIManager::UnregisterButton(button);
			Renderer::UnloadGUIElement(button, 2);
			delete button;
		}

		m_currentItemMenu.Options.clear();
	}

	void ItemSelectionSlot::SetContainedItem(InventoryItem* item)
	{
		m_currentItem = item;
	}

	void ItemSelectionSlot::Render(float zoom)
	{
		GUIButton::Render(zoom);

		//Render the current contents.
		if (m_currentItem != nullptr)
		{
			Sprite* sp = ITEMTYPE_GetItemTypeTexture(m_currentItem->GetType());
			DynamicSprite contentsDisplay(sp->GetSpriteId());
			delete sp;

			Vec2 scale = this->GetScale() * (4.f / 5.f) * Vec2 { -1.f, -1.f };
			contentsDisplay.Render(Vec2{ 0.f, 0.f }, this->GetPosition() + (this->GetScale() / 10.f) - scale, scale);
		}
	}

	void ItemSelectionSlot::onClick(int buttonID)
	{
		for (unsigned int i = 0; i < m_selectionBoxes.size(); i++)
		{
			if (m_selectionBoxes[i]->GetButtonId() == buttonID)
			{
				if (m_selectionBoxes[i]->m_customOnClick != nullptr)
				{
					(*m_selectionBoxes[i]->m_customOnClick)(buttonID);
				}

				if (m_selectionBoxes[i]->m_canBeChanged)
				{
					m_selectionBoxes[i]->SetContainedItem(nullptr);
					m_selectionBoxes[i]->CreateItemSelectionMenu(2, GAME_NAME::TestGame::ThePlayer->GetInventory(), GAME_NAME::TestGame::ThePlayer->GetBackpack());
				}

				return;
			}
		}
	}

	void ItemSelectionSlot::onClickSlotBox(int buttonID)
	{
		for (int i = 0; i < m_currentItemMenu.Options.size(); i++)
		{
			GUIButton*& button = m_currentItemMenu.Options[i];

			if (button->GetButtonId() == buttonID)
			{
				m_currentItemMenu.CurrentBox->SetContainedItem(m_currentItemMenu.Items[i]);

				if (m_currentItemMenu.CurrentBox->m_customOnClick != nullptr)
				{
					(*m_currentItemMenu.CurrentBox->m_customOnClick)(buttonID);
				}

				RemoveItemSelectionMenu();
				return;
			}
		}
	}

}
