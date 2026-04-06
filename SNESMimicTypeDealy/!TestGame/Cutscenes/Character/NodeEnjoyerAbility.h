#pragma once

#include "./CharacterAbility.h"

namespace GAME_NAME::Cutscenes
{
	class NodeEnjoyerAbility
		: public CAbility
	{
	public:
		void Trigger(AnimatingCharacter* character) override;
		std::string GetPrompt() override { return ""; };

		const CharacterAbility GetAbility() { return CharacterAbility::NodeEnjoyer; }

		void Update(AnimatingCharacter* character) override;
	};
}