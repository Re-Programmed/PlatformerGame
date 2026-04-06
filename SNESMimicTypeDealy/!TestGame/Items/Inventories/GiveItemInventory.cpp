#include "../../TestGame.h"
#include "GiveItemInventory.h"
#include "../../../Objects/GUI/GUIManager.h"

namespace GAME_NAME::Items::Inventories
{
	GiveItemInventory::GiveItemMenu GiveItemInventory::m_giveItemMenu;
	std::function<bool(ITEM_TYPE)> GiveItemInventory::m_recieveItemCallback;

	/// <summary>
	/// Checks the proper conditions and appends an item to a selection menu / adds to the count of a previous selection.
	/// </summary>
	/// <param name="selections"></param>
	/// <param name="item"></param>
	void GiveItemInventory_appendSelection(std::vector<GUIItemSelectionBox::ItemSelection>& selections, Inventory::ReturnItem& item)
	{
		if (item.ri_IsNull) { return; }

		const ITEM_TYPE type = item.ri_Item->GetType();

		//Find if the item already exists in the list.
		auto found = std::find_if(selections.begin(), selections.end(), [type](GUIItemSelectionBox::ItemSelection selection) {
			return selection.Type == type;
			});

		//If the item already exists, just increment the count.
		if (found != selections.end())
		{
			selections.at(std::distance(selections.begin(), found)).Count++;
			return;
		}

		//If the item does not exist, add it.
		selections.emplace_back(item.ri_Item->GetType(), 1);
	}

	void GiveItemInventory::Open()
	{
		TestGame::ThePlayer->SetFrozen(true);

		if (m_giveItemMenu.InventorySelectionArea != nullptr)
		{
			Close();
		}
		
		//Make sure there is no callback setup when the inventory is opened, it needs to be set now.
		m_recieveItemCallback = [](ITEM_TYPE item) { return false; };
		
		std::vector<GUIItemSelectionBox::ItemSelection> selections;

		//Create a list of all items in the player backpack.
		const uint8_t size = TestGame::INSTANCE->ThePlayer->GetBackpack()->GetSize();
		for (uint8_t i = 0; i < size; i++)
		{
			Inventory::ReturnItem item = TestGame::INSTANCE->ThePlayer->GetBackpack()->GetItem(i);

			GiveItemInventory_appendSelection(selections, item);
		}
		
		for (uint8_t i = 0; i < 3; i++)
		{
			Inventory::ReturnItem item = TestGame::INSTANCE->ThePlayer->GetInventory()->GetItem(i);

			GiveItemInventory_appendSelection(selections, item);
		}

		m_giveItemMenu.Backing = new GUI::StaticGUIElement(Vec2{ 0.f, 0.f }, Vec2{ TargetResolutionX, TargetResolutionY }, Renderer::GetSpriteIdFromTextureId(SpriteBase(-1)));
		Renderer::LoadGUIElement(m_giveItemMenu.Backing, 1);
		
		m_giveItemMenu.InventorySelectionArea = new GUIItemSelectionBox(Vec2{ TargetResolutionX/3.f - 22.f, TargetResolutionY / 4.f }, (TargetResolutionY/2.f), selections.size() == 0 ? nullptr : &selections[0], selections.size(), new  std::function<void(Items::ITEM_TYPE, int)>(itemSelectionUpdated), false);
		Renderer::LoadGUIElement(m_giveItemMenu.InventorySelectionArea, 2);

		const Vec2 slotScale{ 20.f };
		m_giveItemMenu.SelectedSlot = new ItemSelectionSlot(Vec2{ TargetResolutionX * 2.f / 3.f - slotScale.X / 2.f, TargetResolutionY / 2.f - slotScale.Y / 2.f }, slotScale, Renderer::GetSpriteIdFromTextureId(ITEM_SELECTION_BOX_BG_SPRITE), nullptr, false, nullptr);
		Renderer::LoadGUIElement(m_giveItemMenu.SelectedSlot, 2);

		m_giveItemMenu.GiveButton = new GUIButton(m_giveItemMenu.SelectedSlot->GetPosition() - Vec2{ slotScale.X / 2.f, slotScale.Y }, Vec2{ slotScale.X * 2.f, slotScale.Y/2.f }, Renderer::GetSpriteIdFromTextureId(SpriteBase(-1)), new std::function<void(int)>(itemChosen),
			Vec4{ 0.5f, 1.f, 0.5f, 1.f }, Vec4{ 0.85f, 1.f, 0.85f, 1.f });
		Renderer::LoadGUIElement(m_giveItemMenu.GiveButton, 2);
		GUI::GUIManager::RegisterButton(m_giveItemMenu.GiveButton);

		m_giveItemMenu.GiveText = GUI::Text::TextRenderer::RenderWordCaseSensitive("Give", m_giveItemMenu.GiveButton->GetPosition() + Vec2{ 16.f, 0.25f }, slotScale.Y / 2.f, -2.f, 2);
	}

	void GiveItemInventory::Close()
	{
		TestGame::ThePlayer->SetFrozen(false);

		if (m_giveItemMenu.InventorySelectionArea == nullptr) { return; }

		//The item is memory controlled by this class, so we need to remove it after the inventory is closed.
		if (m_giveItemMenu.SelectedSlot->GetContainedItem())
		{
			delete m_giveItemMenu.SelectedSlot->GetContainedItem();
		}

		if (m_giveItemMenu.GiveText.size() > 0)
		{
			for (GUI::Text::TextRenderer::ExpectedLetter*& letter : m_giveItemMenu.GiveText)
			{
				Renderer::UnloadGUIElement(letter->letter, 2);
				delete letter->letter;
				delete letter;
			}

			m_giveItemMenu.GiveText.clear();
		}

		Renderer::UnloadGUIElement(m_giveItemMenu.InventorySelectionArea, 2);	
		delete m_giveItemMenu.InventorySelectionArea;	
		m_giveItemMenu.InventorySelectionArea = nullptr;

		Renderer::UnloadGUIElement(m_giveItemMenu.SelectedSlot, 2);				
		delete m_giveItemMenu.SelectedSlot;				
		m_giveItemMenu.SelectedSlot = nullptr;

		GUI::GUIManager::UnregisterButton(m_giveItemMenu.GiveButton);
		Renderer::UnloadGUIElement(m_giveItemMenu.GiveButton, 2);				
		delete m_giveItemMenu.GiveButton;				
		m_giveItemMenu.GiveButton = nullptr;

		Renderer::UnloadGUIElement(m_giveItemMenu.Backing, 1);
		delete m_giveItemMenu.Backing;
		m_giveItemMenu.Backing = nullptr;
	}

	void GiveItemInventory::itemSelectionUpdated(ITEM_TYPE item, int count)
	{
		//If the slot has an item, we need to delete it because we create a new item every time the selected item is updated.
		if (m_giveItemMenu.SelectedSlot->GetContainedItem() != nullptr)
		{
			delete m_giveItemMenu.SelectedSlot->GetContainedItem();
		}

		//Set the slot to contain the selected item.
		m_giveItemMenu.SelectedSlot->SetContainedItem(new InventoryItem(item));
	}

	void GiveItemInventory::itemChosen(int buttonID)
	{
		//Run the callback on the selected item.
		if (m_giveItemMenu.SelectedSlot->GetContainedItem())
		{
			ITEM_TYPE item = m_giveItemMenu.SelectedSlot->GetContainedItem()->GetType();

			if (m_recieveItemCallback(item))
			{
				//Remove the item from either the player inventory or backpack.

				bool removed = false;

				for (int i = 0; i < 3; i++)
				{
					auto playerItem = TestGame::ThePlayer->GetInventory()->GetItem(i);
					if (!playerItem.ri_IsNull)
					{
						if (playerItem.ri_Item->GetType() == item)
						{
							delete playerItem.ri_Item;
							TestGame::ThePlayer->GetInventory()->SetItem(i, nullptr);
							return;
						}
					}
				}

				TestGame::ThePlayer->GetBackpack()->Remove(item, 1);
			}
		}
	}

}
