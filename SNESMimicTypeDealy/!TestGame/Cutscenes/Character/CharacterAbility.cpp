#include "CharacterAbility.h"

#include "../DialogueManager.h"
#include "../../Items/Inventories/GiveItemInventory.h"
#include "../../Items/FloorItem.h"

namespace GAME_NAME::Cutscenes
{
	

#pragma region Functional
	FunctionalAbility::FunctionalAbility(std::function<void(AnimatingCharacter*)> func, std::string prompt)
		: m_func(func), m_prompt(prompt)
	{
		
	}

	void FunctionalAbility::Trigger(AnimatingCharacter* character)
	{
		m_func(character);
	}
#pragma endregion

#pragma region Speak
	SpeakAbility::SpeakAbility(DialogueSequence sequence)
		: m_sequence(sequence)
	{

	}

	void SpeakAbility::Trigger(AnimatingCharacter* character)
	{
		DialogueManager::INSTANCE->PlayDialogueSequence(m_sequence);
	}
#pragma endregion
	
#pragma region ItemRecieve
	ItemRecieveAbility::ItemRecieveAbility()
		: CAbility(), RemoveableUpdateable()
	{

	}

	void ItemRecieveAbility::Trigger(AnimatingCharacter* character)
	{
		m_menuOpen = true;
		enableUpdates();

		m_self = character;

		Items::Inventories::GiveItemInventory::Open();
		Items::Inventories::GiveItemInventory::SetItemCallback([this](ITEM_TYPE item) { return this->recieve(item); });
	}

	void ItemRecieveAbility::Update(GLFWwindow* window)
	{
		if (!m_menuOpen) { disableUpdates(); m_menuOpen = false; return; }
	}
#pragma endregion

#pragma region ItemReceiveAndSpeak
	ItemRecieveAndSpeakAbility::ItemRecieveAndSpeakAbility(std::unordered_map<ITEM_TYPE, const SpeechPattern> speechPatterns)
		: m_speechPatterns(speechPatterns)
	{

	}

	bool ItemRecieveAndSpeakAbility::recieve(ITEM_TYPE item)
	{
		if (m_speechPatterns.contains(item))
		{
			Items::Inventories::GiveItemInventory::Close();
			DialogueManager::INSTANCE->PlayDialogueSequence(DialogueSequence(1,
				DialogueSequence::DialogueEvent(m_speechPatterns[item].Say, m_self, 1.f, m_speechPatterns[item].Action)
			));

			if (!m_speechPatterns[item].GiveItemCode.empty())
			{
				InventoryItem* giveItem = InventoryItem::DecodeItemString(m_speechPatterns[item].GiveItemCode);
				if (TestGame::ThePlayer->GetInventory()->AddItem(giveItem) == -1 && TestGame::ThePlayer->GetBackpack()->AddItem(giveItem) == -1)
				{
					FloorItem* floorItem = new FloorItem(m_self->GetPosition() + m_self->GetScale() / 2.f, giveItem, 0.5f);
					Renderer::InstantiateObject(Renderer::InstantiateGameObject(floorItem, true, 2, false));
				}
			}

			return true;
		}

		Items::Inventories::GiveItemInventory::Close();
		
		int randLine = (std::rand() * 3) / RAND_MAX;
		std::string message = "Stop wiggling that in my face!";

		if (randLine == 1)
		{
			message = "Nice " + ITEMTYPE_GetItemTypeName(item) + " %item_" + std::to_string(item) + " .";
		}
		else if (randLine == 2)
		{
			message = "Oh its an "+ ITEMTYPE_GetItemTypeName(item) + " %item_" + std::to_string(item) + " , like I haven't seen those before...";
		}

		DialogueManager::INSTANCE->PlayDialogueSequence(DialogueSequence(1,
			DialogueSequence::DialogueEvent(message, m_self, 1.f, Objects::Player::Player::PLAYER_ANIMATION_STATE::NO_LOOK_DIRECTION)
		));
		return false;
	}
#pragma endregion
}

