#include "CharacterAbility.h"

#include "../DialogueManager.h"
#include "../../Items/Inventories/GiveItemInventory.h"
#include "../../Items/FloorItem.h"

#include "../../Objects/Environment/Effects/Explosion.h"

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
		//Check if the character is desiring a specific item right now.
		CAbility* desiredItemAbility = m_self->GetAbility(CharacterAbility::DesiredItem);

		if (desiredItemAbility)
		{
			//Try to give the desired item.
			if ((dynamic_cast<DesiredItemAbility*>(desiredItemAbility))->Give(item))
			{
				Items::Inventories::GiveItemInventory::Close();
				return true;
			}
		}

		if (m_speechPatterns.contains(item))
		{
			Items::Inventories::GiveItemInventory::Close();
			DialogueManager::INSTANCE->PlayDialogueSequence(DialogueSequence(1,
				DialogueSequence::DialogueEvent(m_speechPatterns[item].Say.starts_with(">") ? (DialogueManager::INSTANCE->GetPhrase(m_speechPatterns[item].Say.substr(1), item)) : m_speechPatterns[item].Say, m_self, 1.f, m_speechPatterns[item].Action)
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

		if (randLine == 0)
		{
			DialogueManager::INSTANCE->PlayDialogueSequence(DialogueManager::INSTANCE->GetDialogueSequence("AnimatingCharacter_UnknownItem1", item));
		}else if (randLine == 1)
		{
			DialogueManager::INSTANCE->PlayDialogueSequence(DialogueManager::INSTANCE->GetDialogueSequence("AnimatingCharacter_UnknownItem2", item));
		}
		else if (randLine == 2)
		{
			DialogueManager::INSTANCE->PlayDialogueSequence(DialogueManager::INSTANCE->GetDialogueSequence("AnimatingCharacter_UnknownItem3", item));
		}

		return false;
	}
#pragma endregion

#pragma region PlayAs
	PlayAsAbility::PlayAsAbility(Objects::Player::Player::TEXTURE_OFFSETS character, bool available)
		: m_character(character), m_available(available)
	{

	}

	void PlayAsAbility::Trigger(AnimatingCharacter* character)
	{
		if (!m_available)
		{ 
			int randLine = (std::rand() * 3) / RAND_MAX;

			if (randLine == 0)
			{
				DialogueManager::INSTANCE->PlayDialogueSequence(DialogueManager::INSTANCE->GetDialogueSequence(">AnimatingCharacter_CannotSwitch1"));
			}
			else if (randLine == 1)
			{
				DialogueManager::INSTANCE->PlayDialogueSequence(DialogueManager::INSTANCE->GetDialogueSequence(">AnimatingCharacter_CannotSwitch2"));
			}
			else if (randLine == 2)
			{
				DialogueManager::INSTANCE->PlayDialogueSequence(DialogueManager::INSTANCE->GetDialogueSequence(">AnimatingCharacter_CannotSwitch3"));
			}

			return;
		}

		Explosion* playerExplosion = new Explosion(TestGame::ThePlayer->GetPosition() + TestGame::ThePlayer->GetScale()/2.f, 20.f);
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(playerExplosion, true, 2, true));
		Explosion* characterExplosion = new Explosion(character->GetPosition() + character->GetScale()/2.f, 20.f);
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(characterExplosion, true, 2, true));


		TestGame::ThePlayer->SetPlayerTextureData(this->m_character);
	}

#pragma endregion

#pragma region DesiredItem
	DesiredItemAbility::DesiredItemAbility(ITEM_TYPE item, std::string givenSequenceCode, std::string giveItemCode)
		: m_item(item), m_givenSequence(givenSequenceCode), m_giveItemCode(giveItemCode)
	{

	}

	void DesiredItemAbility::Trigger(AnimatingCharacter* character)
	{
		//pass
	}
	bool DesiredItemAbility::Give(ITEM_TYPE attempt)
	{
		if (m_given) { return false; }

		if (m_item == attempt)
		{
			if (!DialogueManager::INSTANCE->IsSequencePlaying())
			{
				DialogueManager::INSTANCE->PlayDialogueSequence(DialogueManager::INSTANCE->GetDialogueSequence(this->m_givenSequence, attempt));
			}
			
			InventoryItem* giveItem = InventoryItem::DecodeItemString(m_giveItemCode);
			FloorItem* floorItem = new FloorItem(TestGame::ThePlayer->GetPosition() + TestGame::ThePlayer->GetScale() / 2.f, giveItem, 0.5f);
			Renderer::InstantiateObject(Renderer::InstantiateGameObject(floorItem, true, 2, false));
			m_given = true;
			return true;
		}

		return false;
	}

	//Render the desired item above the character's head.
	void DesiredItemAbility::Render(const Vec2& cameraPos, AnimatingCharacter* character)
	{
		if (m_given) { return; }

		Sprite* sprite = ITEMTYPE_GetItemTypeTexture(m_item);
		sprite->Render(cameraPos, character->GetPosition() + Vec2{ character->GetScale().X / 2.f - 4.f, character->GetScale().Y + 3.f }, Vec2{ 8.f });
		delete sprite;
	}
#pragma endregion
}

