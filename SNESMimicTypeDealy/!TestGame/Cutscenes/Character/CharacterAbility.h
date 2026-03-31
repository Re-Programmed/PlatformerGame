#pragma once
#include <string>

#include "../DialogueSequence.h"

#include "../../Items/ItemType.h"
#include "../../../Utils/RemoveableUpdateable.h"


namespace GAME_NAME::Cutscenes
{
class AnimatingCharacter;

	/// <summary>
	/// Something an animating character can do.
	/// </summary>
	enum class CharacterAbility
	{
		Functional		= 0b00000001,
		CanSpeak		= 0b00000010,
		CanRecieveItem	= 0b00000100
	};

	/// <summary>
	/// What the different abilities do.
	/// </summary>
	class CAbility
	{
	public:
		virtual void Trigger(AnimatingCharacter* character) = 0;
		virtual std::string GetPrompt() = 0;

		virtual const CharacterAbility GetAbility() = 0;
	};

	/// <summary>
	/// An ability that will call a customizable function.
	/// </summary>
	class FunctionalAbility
		: public CAbility
	{
		FunctionalAbility(std::function<void(AnimatingCharacter*)> func, std::string prompt = "");

		void Trigger(AnimatingCharacter* character) override;
		inline std::string GetPrompt() override { return m_prompt; }

		const CharacterAbility GetAbility() override { return CharacterAbility::Functional; }
	private:
		const std::function<void(AnimatingCharacter*)> m_func;
		const std::string m_prompt;
	};

	/// <summary>
	/// An ability that allows the player to interact and talk with this character.
	/// </summary>
	class SpeakAbility
		: public CAbility
	{
	public:
		SpeakAbility(DialogueSequence sequence);

		void Trigger(AnimatingCharacter* character) override;
		inline std::string GetPrompt() override { return "Speak"; }

		const CharacterAbility GetAbility() override { return CharacterAbility::CanSpeak; }
	private:
		const DialogueSequence m_sequence;
	};

using namespace GAME_NAME::Utils;
	class ItemRecieveAbility
		: public CAbility, public RemoveableUpdateable
	{
	public:
		ItemRecieveAbility();

		void Trigger(AnimatingCharacter* character) override;
		inline std::string GetPrompt() override { return "Give"; }

		void Update(GLFWwindow* window) override;

		const CharacterAbility GetAbility() override { return CharacterAbility::CanRecieveItem; }
	protected:
		/// <summary>
		/// A pointer to the character this is attached to.
		/// </summary>
		AnimatingCharacter* m_self = nullptr;

		virtual bool recieve(ITEM_TYPE item) = 0;

	private:
		bool m_menuOpen = false;
	};

	class ItemRecieveAndSpeakAbility
		: public ItemRecieveAbility
	{
	public:
		typedef struct SpeechPattern {
			std::string Say;
			Objects::Player::Player::PLAYER_ANIMATION_STATE Action;
			std::string GiveItemCode;
		};

		ItemRecieveAndSpeakAbility(std::unordered_map<ITEM_TYPE, const SpeechPattern> speechPatterns);

	private:
		/// <summary>
		/// What a character will say when given various items.
		/// </summary>
		std::unordered_map<ITEM_TYPE, const SpeechPattern> m_speechPatterns;
		
	protected:
		bool recieve(ITEM_TYPE item) override;
	};
}