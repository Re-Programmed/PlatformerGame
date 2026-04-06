#pragma once

#include "../../../Objects/Player/Player.h"
#include "../AnimatingCharacter.h"
#include "../NodeEnjoyerAbility.h"

#include "../../DialogueManager.h"

namespace GAME_NAME::Cutscenes::LobbyCharacters
{
	class FopsLobby
	{
	public:
		static void Load()
		{
			Objects::Player::Player::PlayerTextureData* data = new Objects::Player::Player::PlayerTextureData(Objects::Player::Player::TextureData[Objects::Player::Player::TEXTURE_OFFSETS::DEFAULT_FOXO]);
			Cutscenes::AnimatingCharacter* character = new Cutscenes::AnimatingCharacter(Vec2{ 50.f, 70.f }, Vec2{ 26.f, 26.f }, false, 7.f, data);

			character->AddAbility(new Cutscenes::SpeakAbility(Cutscenes::DialogueSequence(1,
				Cutscenes::DialogueSequence::DialogueEvent("Hey!!", character, 1.f, Objects::Player::Player::BAG)
			)));

			character->AddAbility(new Cutscenes::ItemRecieveAndSpeakAbility({
				{ ITEM_TYPE::APPLE, Cutscenes::ItemRecieveAndSpeakAbility::SpeechPattern{ ">AnimatingCharacter_Food1", Objects::Player::Player::NO_LOOK_DIRECTION, ""}},
				{ ITEM_TYPE::TWIG, Cutscenes::ItemRecieveAndSpeakAbility::SpeechPattern{ ">AnimatingCharacter_CraftWooden1", Objects::Player::Player::NO_LOOK_DIRECTION, "w6+100" }}
				}));

			character->AddAbility(new Cutscenes::PlayAsAbility(Objects::Player::Player::DEFAULT_FOXO, false));

			//TODO: randomize this item on level change.
			character->AddAbility(new Cutscenes::DesiredItemAbility(ITEM_TYPE::GLUTEN_FREE_BREAD, "AnimatingCharacter_DesiredItem", "i64" /*Wooden bench*/));

			character->AddAbility(new Cutscenes::NodeEnjoyerAbility());

			Renderer::LoadActiveObject(character, 3);
		}
	};
}