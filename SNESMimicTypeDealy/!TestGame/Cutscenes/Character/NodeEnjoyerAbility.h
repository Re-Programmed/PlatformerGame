#pragma once

#include "./CharacterAbility.h"
#include "ICharacterNode.h"

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

	private:
		/// <summary>
		/// Used to walk to the node before using it.
		/// </summary>
		ICharacterNode* m_targetNode = nullptr;
		ICharacterNode* m_usingNode = nullptr;

		double m_timeSinceLastInteraction = 0.0;

		/// <summary>
		/// If the character is taking too long to get somewhere, just give up.
		/// </summary>
		double m_travelTimeTimeout = 0.0;
	};
}