#include "Backpack.h"

#include "../../../../Objects/GUI/GUIButton.h"
#include "../../../../Objects/GUI/GUIManager.h"

#include "../../../../Utils/Time/GameTime.h"

#include "../../../../Input/InputManager.h"
#include "../../../../CustomCompileOptions.h"

#include "../../../TestGame.h"

#include "../../../../Utils/CollisionDetection.h"

#include "../../../Items/Inventories/InventoryTooltip.h"

#include "../../../../Audio/SoundEvents.h"

using namespace GAME_NAME::Items::Inventories;

#define BACKPACK_SLOT_ITEM_SCALEDOWN 3.0f

namespace GAME_NAME::Objects::Player
{
	Backpack* Backpack::CurrentOpenBackpack = nullptr;

	std::vector<uint32_t> Backpack::m_buttonIDSync;

	InventoryItem* Backpack::m_cursorItem = nullptr;
	StaticGUIElement* Backpack::m_cursorItemDisplay = nullptr;

	StaticGUIElement* Backpack::m_playerSlots[3] { nullptr, nullptr, nullptr };

	bool Backpack::m_animating = false;
	std::mutex Backpack::m_animatingMutex;

	double Backpack::m_clickDelay = 0.0;

	Backpack::Backpack(uint8_t size)
		: Inventory("Backpack", size + BACKPACK_NUM_EQUIPMENT_SLOTS), MiscStateGroup("bk"), m_equipmentSlots{ nullptr, nullptr, nullptr }, m_equipmentDisplayItems{ nullptr, nullptr, nullptr }
	{
		//TESTING, ADD INVENTORY SIZE AND ITEMS TO INVENTORY.
		m_size = size + BACKPACK_NUM_EQUIPMENT_SLOTS;

		//Allocate null to all equipment slots.
		for (uint8_t i = 0; i < BACKPACK_NUM_EQUIPMENT_SLOTS; i++)
		{
			SetItem(i, nullptr);
		}

		///TODO: LOAD STATES AND READ FROM SAVE.
		std::shared_ptr<std::vector<std::string>> data = getStates();

		if (data->size() > 0)
		{			
			//Iterate over all save states, loading the corresponding item.
			for (int i = 0; i < data->size(); i++)
			{
				InventoryItem* ii = InventoryItem::DecodeItemString(data->at(i));
				if (ii != nullptr) { SetItem(i, ii); }
			}
		}
	}

	bool Backpack::Open(bool ignoreAnimation)
	{
		if (GUI::GUIManager::PreventMenus) { return false; }
		if (m_animating && (m_generalSlots.size() > 0 && m_generalSlots[0] != nullptr)) { return false; }

		GUI::GUIManager::PreventMenus = true;

		CurrentOpenBackpack = this;

		m_buttonIDSync.clear();

		StaticGUIElement* equipmentSlotsBacking = new StaticGUIElement(Vec2(ignoreAnimation ? 325.f : 366.f, 26.5f), Vec2(-40.f, 64.f), Renderer::GetSprite(SpriteBase(102))->GetSpriteId());
		Renderer::LoadGUIElement(equipmentSlotsBacking);

		if (!ignoreAnimation)
		{
			m_animatingMutex.lock();
			m_animating = true;
			m_animatingMutex.unlock();

			//Smoothly bring the equipment backing and slots onto the screen.
			m_isOpen = true;
			std::thread moveInThread([equipmentSlotsBacking, this]() {
				while (equipmentSlotsBacking->GetPosition().X > 326.f && m_isOpen)
				{
					equipmentSlotsBacking->SetPosition(Vec2(std::lerp(equipmentSlotsBacking->GetPosition().X, 325.f, 0.01f), equipmentSlotsBacking->GetPosition().Y));

					//Smooth the equipment slots on too.
					for (uint8_t i = 0; i < BACKPACK_NUM_EQUIPMENT_SLOTS; i++)
					{
						if (m_equipmentSlots[i] != nullptr)
						{
							m_equipmentSlots[i]->SetPosition(Vec2(std::lerp(m_equipmentSlots[i]->GetPosition().X, 290.f, 0.01f), m_equipmentSlots[i]->GetPosition().Y));
						}

						//Scale in the items in the equipment slots.
						if (m_equipmentDisplayItems[i] != nullptr)
						{
							m_equipmentDisplayItems[i]->SetScale(Vec2(static_cast<float>(std::lerp(m_equipmentDisplayItems[i]->GetScale().X, 16.f - BACKPACK_SLOT_ITEM_SCALEDOWN, 0.01)), static_cast<float>(std::lerp(m_equipmentDisplayItems[i]->GetScale().Y, 16.f - BACKPACK_SLOT_ITEM_SCALEDOWN, 0.01))));
						}
					}

					//Fade in general slots.
					for (uint8_t i = 0; i < m_generalSlots.size(); i++)
					{
						Vec4 baseColor = m_generalSlots[i]->GetBaseColor();

						//TODO: Add MUTEX FOR SLOTS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

						m_generalSlots[i]->SetBaseColor(Vec4::LerpBetween(baseColor, { 1.f, 1.f, 1.f, 1.f }, 0.05));
						/*
						Vec2 previousScale = m_generalSlots[i]->GetScale();
						m_generalSlots[i]->SetScale({ static_cast<float>(std::lerp(previousScale.X, 20.f, 0.15)), static_cast<float>(std::lerp(previousScale.Y, 20.f, 0.05)) });
						m_generalSlots[i]->SetPosition({ m_generalSlots[i]->GetPosition().X + (previousScale.X - m_generalSlots[i]->GetScale().X)/2.f, m_generalSlots[i]->GetPosition().Y + (previousScale.Y - m_generalSlots[i]->GetScale().Y) / 2.f});*/
					}

					std::this_thread::sleep_for(std::chrono::microseconds(1));
				}

				//We are done animating.
				m_animatingMutex.lock();
				m_animating = false;
				m_animatingMutex.unlock();

			});
			moveInThread.detach();
		}
		else {
			m_isOpen = true;
		}

		m_displayFeatures.push_back(equipmentSlotsBacking);

		//Create slots for backpack size.
		for (uint8_t i = 0; i < m_size - BACKPACK_NUM_EQUIPMENT_SLOTS; i++)
		{
			GUIButton* backpackItemSlot = new GUIButton(Vec2(20.f + ((i % 5) * 19.f), 140.f - (19.f * (i / 5))), Vec2(20.f, 20.f), Renderer::GetSprite(SpriteBase(229))->GetSpriteId(), new std::function(clickGeneralItemSlot), {1.f, 1.f, 1.f, ignoreAnimation ? 1.f : 0.f});
			Renderer::LoadGUIElement(backpackItemSlot, 2);
			m_buttonIDSync.push_back(GUIManager::RegisterButton(backpackItemSlot));

			m_generalSlots.push_back(backpackItemSlot);

			const int index = BACKPACK_NUM_EQUIPMENT_SLOTS + i;

			if (m_items.size() > index && m_items[index] != nullptr)
			{
				StaticGUIElement* itemDisplay = new StaticGUIElement(Vec2(backpackItemSlot->GetPosition().X + 10.f /* Slot scale/2 */ - (16.f - BACKPACK_SLOT_ITEM_SCALEDOWN)/2.f, backpackItemSlot->GetPosition().Y + 10.f - (16.f - BACKPACK_SLOT_ITEM_SCALEDOWN)/2.f), Vec2(16.f - BACKPACK_SLOT_ITEM_SCALEDOWN, 16.f - BACKPACK_SLOT_ITEM_SCALEDOWN), ITEMTYPE_GetItemTypeTexture(m_items[index]->GetType())->GetSpriteId());
				Renderer::LoadGUIElement(itemDisplay, 2);

				m_generalDisplayItems.push_back(itemDisplay);
			}
			else {
				m_generalDisplayItems.push_back(nullptr);
			}
		}

		loadEquipmentSlots(ignoreAnimation);

		//Player Slots.
		createPlayerSlots();

		InventoryTooltip::CreateTooltip();

		return true;
	}

	bool Backpack::Close(bool ignoreAnimation)
	{
		if (m_animating) { return false; }

		GUI::GUIManager::PreventMenus = false;

		if (CurrentOpenBackpack == this)
		{
			CurrentOpenBackpack = nullptr;
		}

		m_buttonIDSync.clear();

		if (!ignoreAnimation)
		{
			m_animatingMutex.lock();
			m_animating = true;
			m_animatingMutex.unlock();
			m_isOpen = false;


			//Remove all item textures before playing fade animation.
			for (uint8_t i = 0; i < m_generalDisplayItems.size(); i++)
			{
				if (m_generalDisplayItems[i] != nullptr)
				{
					Renderer::UnloadGUIElement(m_generalDisplayItems[i], 2);
					delete m_generalDisplayItems[i];
				}
			}

			//Unregister buttons to prevent hovering changing the button texture while fading.
			for (uint8_t i = 0; i < m_generalSlots.size(); i++)
			{
				GUIManager::UnregisterButton(m_generalSlots[i]);
				m_generalSlots[i]->SetHoverColor(Vec4{ 0.f, 0.f, 0.f, 0.f });
			}

			m_generalDisplayItems.clear();

			//Smoothly exit the equipment backing and slots off the screen.
			std::thread moveOutThread([this]() {

				//Move everything away.
				while (m_displayFeatures[0]->GetPosition().X < 365.f && !m_isOpen)
				{
					//Smooth all other display features off.
					for (int i = 0; i < m_displayFeatures.size(); i++)
					{
						m_displayFeatures[i]->SetPosition(Vec2(std::lerp(m_displayFeatures[i]->GetPosition().X, 366.f, 0.01f), m_displayFeatures[i]->GetPosition().Y));
					}

					//Smooth the equipment slots off too.
					for (uint8_t i = 0; i < BACKPACK_NUM_EQUIPMENT_SLOTS; i++)
					{
						if (m_equipmentSlots[i] != nullptr)
						{
							m_equipmentSlots[i]->SetPosition(Vec2(std::lerp(m_equipmentSlots[i]->GetPosition().X, 326.f, 0.01f), m_equipmentSlots[i]->GetPosition().Y));
						}
					}

					//Fade out general slots.
					for (uint8_t i = 0; i < m_generalSlots.size(); i++)
					{
						m_generalSlots[i]->SetBaseColor(Vec4::LerpBetween(m_generalSlots[i]->GetBaseColor(), { 1.f, 1.f, 1.f, 0.f }, 0.05));
					}

					std::this_thread::sleep_for(std::chrono::microseconds(1));
				}

				m_animatingMutex.lock();
				m_animating = false;
				m_animatingMutex.unlock();

				//Delete all equipment slots.
				for (uint8_t i = 0; i < BACKPACK_NUM_EQUIPMENT_SLOTS; i++)
				{
					Renderer::UnloadGUIElement(m_equipmentSlots[i], 2);
					GUIManager::UnregisterButton(m_equipmentSlots[i]);

					delete m_equipmentSlots[i];
					m_equipmentSlots[i] = nullptr;

					if (m_equipmentDisplayItems[i] != nullptr)
					{
						Renderer::UnloadGUIElement(m_equipmentDisplayItems[i], 2);
						delete m_equipmentDisplayItems[i];
						m_equipmentDisplayItems[i] = nullptr;
					}
				}

				for (int i = 0; i < m_displayFeatures.size(); i++)
				{
					Renderer::UnloadGUIElement(m_displayFeatures[i]);

					delete m_displayFeatures[i];
				}

				for (uint8_t i = 0; i < m_generalSlots.size(); i++)
				{
					Renderer::UnloadGUIElement(m_generalSlots[i], 2);

					delete m_generalSlots[i];
				}

				m_displayFeatures.clear();
				m_generalSlots.clear();
				});
			moveOutThread.detach();
		}
		else
		{
			//Delete all equipment slots.
			for (uint8_t i = 0; i < BACKPACK_NUM_EQUIPMENT_SLOTS; i++)
			{
				Renderer::UnloadGUIElement(m_equipmentSlots[i], 2);
				GUIManager::UnregisterButton(m_equipmentSlots[i]);

				delete m_equipmentSlots[i];
				m_equipmentSlots[i] = nullptr;

				if (m_equipmentDisplayItems[i] != nullptr)
				{
					Renderer::UnloadGUIElement(m_equipmentDisplayItems[i], 2);
					delete m_equipmentDisplayItems[i];
					m_equipmentDisplayItems[i] = nullptr;
				}
			}

			for (int i = 0; i < m_displayFeatures.size(); i++)
			{
				Renderer::UnloadGUIElement(m_displayFeatures[i]);

				delete m_displayFeatures[i];
			}

			for (uint8_t i = 0; i < m_generalSlots.size(); i++)
			{
				Renderer::UnloadGUIElement(m_generalSlots[i], 2);
				GUIManager::UnregisterButton(m_generalSlots[i]);

				delete m_generalSlots[i];
			}

			for (uint8_t i = 0; i < m_generalDisplayItems.size(); i++)
			{
				if (m_generalDisplayItems[i] != nullptr)
				{
					Renderer::UnloadGUIElement(m_generalDisplayItems[i], 2);
					delete m_generalDisplayItems[i];
				}
			}

			m_generalDisplayItems.clear();
			m_displayFeatures.clear();
			m_generalSlots.clear();

			m_isOpen = false;
		}

		InventoryTooltip::RemoveTooltip();

		removePlayerSlots();

		//Update current save states.
		clearStates();
		for (int i = 0; i < m_items.size(); i++)
		{
			if (m_items[i] == nullptr)
			{
				//Item was null, add temp NULL_ITEM.
				assignState(new InventoryItem(NULL_ITEM));
			}
			else {
				assignState(m_items[i]);
			}
		}

		return true;
	}

	void Backpack::Render()
	{
		if (m_clickDelay > 0.0) { m_clickDelay -= Utils::Time::GameTime::GetScaledDeltaTime(); }
		UpdateCursorItemDisplay();
	}


	void Backpack::UpdateCursorItemDisplay()
	{
		if (m_cursorItemDisplay != nullptr)
		{
			m_cursorItemDisplay->SetPosition(InputManager::GetMouseScreenPosition() - Vec2(0.f, 12.f));
		}

		if (m_isOpen)
		{
			//Equipment slots.
			for (int i = 0; i < 3; i++)
			{
				GUIButton*& slot = m_equipmentSlots[i];

				ReturnItem item = this->GetItem(i);

				//Slot is hovered.
				if (!item.ri_IsNull && Utils::CollisionDetection::PointWithinBoxBL(InputManager::GetMouseScreenPosition(), slot->GetPosition(), slot->GetScale()))
				{
					InventoryTooltip::UpdateTooltip(i, item, false, true);
					return;
				}
			}

			//Player slots.
			for (int i = 0; i < 3; i++)
			{
				StaticGUIElement*& slot = m_playerSlots[i];

				ReturnItem item = TestGame::ThePlayer->GetInventory()->GetItem(i);

				//Slot is hovered.
				if (!item.ri_IsNull && Utils::CollisionDetection::PointWithinBoxBL(InputManager::GetMouseScreenPosition(), slot->GetPosition(), slot->GetScale()))
				{
					InventoryTooltip::UpdateTooltip(i, item, false);
					return;
				}
			}

			//General slots tooltip.
			for (int i = 0; i < m_generalSlots.size(); i++)
			{
				GUIButton*& slot = m_generalSlots[i];

				ReturnItem item = this->GetItem(i + 3 /*First 3 slots are equipment slots, skip over these.*/);

				//Slot is hovered.
				if (!item.ri_IsNull && Utils::CollisionDetection::PointWithinBoxBL(InputManager::GetMouseScreenPosition(), slot->GetPosition(), slot->GetScale()))
				{
					InventoryTooltip::UpdateTooltip(i, item, false);
					return;
				}
			}

			InventoryTooltip::UpdateTooltip(0, {}, true);
		}
	}

	int Backpack::AddItem(InventoryItem* item)
	{
		for (int i = BACKPACK_NUM_EQUIPMENT_SLOTS; i < this->m_size; i++)
		{
			if (this->m_items[i] == nullptr || this->m_items[i]->GetType() == ITEM_TYPE::NULL_ITEM)
			{
				SetItem(i, item);
				return i;
			}
		}

		return -1;
	}


	StaticGUIElement* Backpack::setCursorItem(InventoryItem* item)
	{
		if (m_cursorItem != nullptr) { return nullptr; }
		if (item == nullptr) { return nullptr; }

		m_cursorItem = item;
		m_cursorItemDisplay = new StaticGUIElement(InputManager::GetMouseScreenPosition() - Vec2(0.f, 12.f), Vec2(12.f), ITEMTYPE_GetItemTypeTexture(item->GetType())->GetSpriteId());

		return m_cursorItemDisplay;
	}

	void Backpack::loadEquipmentSlots(const bool& ignoreAnimation)
	{

		for (uint8_t i = 0; i < BACKPACK_NUM_EQUIPMENT_SLOTS; i++)
		{
			GUIButton* slot = new GUIButton(Vec2(ignoreAnimation ? 290.f : 326.f, ((BACKPACK_NUM_EQUIPMENT_SLOTS - i) * 20.f) + 10.f), Vec2(16.f, 16.f), Renderer::GetSprite(SpriteBase(101))->GetSpriteId(), new std::function(clickEquipmentSlot), Vec4(0.75f, 0.75f, 0.75f, 1.f), Vec4(1.f, 1.f, 1.f, 1.f));
			Renderer::LoadGUIElement(slot, 2);
			m_buttonIDSync.push_back(GUIManager::RegisterButton(slot));

			m_equipmentSlots[i] = slot;

			//Create item displays.
			if (m_items.size() > i)
			{
				if (m_items[i] == nullptr) { continue; }
				StaticGUIElement* itemDisplay = new StaticGUIElement(Vec2(ignoreAnimation ? (290.f + BACKPACK_SLOT_ITEM_SCALEDOWN / 2.f) : 293.f, slot->GetPosition().Y + BACKPACK_SLOT_ITEM_SCALEDOWN / 2.f), ignoreAnimation ? Vec2(16.f - BACKPACK_SLOT_ITEM_SCALEDOWN) : Vec2(0.f), ITEMTYPE_GetItemTypeTexture(m_items[i]->GetType())->GetSpriteId());
				Renderer::LoadGUIElement(itemDisplay, 2);

				m_equipmentDisplayItems[i] = itemDisplay;
			}
		}
	}
	


	void Backpack::clickEquipmentSlot(int id)
	{
		if (m_clickDelay > 0.f) { return; }

		for (int i = 0; i < m_buttonIDSync.size(); i++)
		{
			if (m_buttonIDSync[i] == id)
			{
				id = i;
				break;
			}
		}

		//TODO: gui_equipment sound.

		m_clickDelay = 0.25f;

		if (CurrentOpenBackpack == nullptr) { return; }
		updateSlotItem(id - (CurrentOpenBackpack->m_size - BACKPACK_NUM_EQUIPMENT_SLOTS), TOOL_ACTION::EQUIPMENT /*Item must have the equipment attribute to enter this slot.*/);
	}

	void Backpack::clickGeneralItemSlot(int id)
	{
		if (m_clickDelay > 0.f) { return; }

		for (int i = 0; i < m_buttonIDSync.size(); i++)
		{
			if (m_buttonIDSync[i] == id)
			{
				id = i;
				break;
			}
		}


		m_clickDelay = 0.25f;

		Audio::SoundEvents::PlaySoundGlobal(Audio::SoundEvents::Event::GUI_ITEM, 0.66f);

		const int& index = id + BACKPACK_NUM_EQUIPMENT_SLOTS;

		updateSlotItem(index);
	}

#define UpdateBag() {										\
		Backpack* const savedBag = CurrentOpenBackpack;		\
		CurrentOpenBackpack->Close(true);					\
		savedBag->Open(true);								\
	}														\

	void Backpack::clickPlayerSlot(int id)
	{
		if (m_clickDelay > 0.f) { return; }

		m_clickDelay = 0.25f;

		for (int i = 0; i < m_buttonIDSync.size(); i++)
		{
			if (m_buttonIDSync[i] == id)
			{
				id = i;
				break;
			}
		}

		const int& index = id - CurrentOpenBackpack->m_size;
		StaticGUIElement* cursorElement = nullptr;

		ReturnItem playerSlotItem = TestGame::ThePlayer->GetInventory()->GetItem(index);

		if (cursorElement = setCursorItem(TestGame::ThePlayer->GetInventory()->GetItem(index).ri_Item))
		{
			TestGame::ThePlayer->GetInventory()->SetItem(index, nullptr);
		}
		else {
			if (m_cursorItem == nullptr) { return; }
			TestGame::ThePlayer->GetInventory()->SetItem(index, m_cursorItem);

			Renderer::UnloadGUIElement(m_cursorItemDisplay, 2);
			delete m_cursorItemDisplay;
			m_cursorItemDisplay = nullptr;

			m_cursorItem = nullptr;

			if (!playerSlotItem.ri_IsNull)
			{
				cursorElement = setCursorItem(playerSlotItem.ri_Item);
			}
		}

		Audio::SoundEvents::PlaySoundGlobal(Audio::SoundEvents::Event::GUI_ITEM, 0.66f);

		UpdateBag();

		//Load cursor element after to keep it in front.
		Renderer::LoadGUIElement(cursorElement, 2);
	}

	void Backpack::updateSlotItem(const int& index, int requiredToolAction)
	{
		InventoryItem* const slotItem = CurrentOpenBackpack->m_items.size() <= index ? nullptr : CurrentOpenBackpack->m_items[index];
		StaticGUIElement* cursorElement = nullptr;

		//Either take or put an item based on what is in the cursor.
		if (cursorElement = setCursorItem(slotItem))
		{
			CurrentOpenBackpack->SetItem(index, nullptr);
		} else {
		
			if (m_cursorItem == nullptr) { return; }

			//Check if the held tool has the required ability to enter this slot.
			if (requiredToolAction > 0)
			{
				if ((ITEMTYPE_GetItemData(m_cursorItem->GetType()).Actions & requiredToolAction) == 0)
				{
					return;
				}
			}

			CurrentOpenBackpack->SetItem(index, m_cursorItem);

			Renderer::UnloadGUIElement(m_cursorItemDisplay, 2);
			delete m_cursorItemDisplay;
			m_cursorItemDisplay = nullptr;

			m_cursorItem = nullptr;

			if (slotItem != nullptr)
			{
				cursorElement = setCursorItem(slotItem);
			}
		}

		//Update current backpack.
		UpdateBag();

		//Load cursor element after to keep it in front.
		Renderer::LoadGUIElement(cursorElement, 2);
	}

	void Backpack::createPlayerSlots()
	{
		for (int i = 0; i < 3; i++)
		{
			GUIButton* playerSlot = new GUIButton({ 2.98f + (i * 20.f), 19.f }, { 18.f, 18.f }, Renderer::GetSprite(INVENTORY_SELECTED_SLOT_SPRITE)->GetSpriteId(), new std::function(clickPlayerSlot), { 1.f, 1.f, 1.f, 0.f }, { 0.5f, 0.5f, 0.f, 0.25f });
			Renderer::LoadGUIElement(playerSlot, 2);
			m_buttonIDSync.push_back(GUIManager::RegisterButton(playerSlot));

			m_playerSlots[i] = playerSlot;
		}
	}

	void Backpack::removePlayerSlots()
	{
		for (int i = 0; i < 3; i++)
		{
			Renderer::UnloadGUIElement(m_playerSlots[i], 2);
			GUIManager::UnregisterButton(dynamic_cast<GUIButton*>(m_playerSlots[i]));
			delete m_playerSlots[i];
			m_playerSlots[i] = nullptr;
		}
	}
}