#pragma once

#include "../../Objects/GameObject.h"
#include <queue>
#include <cstdarg>

#include "../Objects/Player/Player.h"

using namespace GAME_NAME::Objects;

namespace GAME_NAME::Cutscenes
{
typedef Objects::Player::Player::PLAYER_ANIMATION_STATE AnimationState;

	class DialogueSequence
	{
	public:
		struct DialogueEvent
		{
			std::string Text;
			GameObject* FocusObject;

			float Zoom;
			std::vector<std::shared_ptr<DialogueSequence>> Options;

			AnimationState SpeakerAnimation = AnimationState::NO_LOOK_DIRECTION;

			DialogueEvent(std::string text, GameObject* focusObject = nullptr, float zoom = 1.f, Objects::Player::Player::PLAYER_ANIMATION_STATE speakerAnimation = Objects::Player::Player::NO_LOOK_DIRECTION)
				: Text(text), FocusObject(focusObject), Zoom(zoom), SpeakerAnimation(speakerAnimation)
			{

			}

			DialogueEvent(std::string text, GameObject* focusObject, float zoom, int numOptions, /*std::shared_ptr<DialogueSequence> options*/...)
				: Text(text), FocusObject(focusObject), Zoom(zoom)
			{
				va_list args;
				va_start(args, numOptions);

				for (unsigned int i = 0; i < numOptions; i++)
				{
					Options.emplace_back(va_arg(args, std::shared_ptr<DialogueSequence>));
				}

				va_end(args);
			}

			inline void SetFocusObject(GameObject*& focus)
			{
				FocusObject = focus;
			}

			inline void SetZoom(const float& zoom)
			{
				Zoom = zoom;
			}

			void AddOption(const std::string& option);

			inline void AddOption(std::shared_ptr<DialogueSequence>& option)
			{
				Options.push_back(option);
			}
		};

		inline bool IsEmpty() { return this->m_dialogueEvents.empty(); }

		/// <summary>
		/// Create an empty sequence.
		/// </summary>
		DialogueSequence()
		{

		}

		/// <summary>
		/// Creates a dialogue sequence with the given events.
		/// </summary>
		/// <param name="numEvents">How many dialogue events are listed.</param>
		/// <param name="...">A list of dialogue events.</param>
		DialogueSequence(unsigned int numEvents, ...)
		{
			va_list args;
			va_start(args, numEvents);
			
			for (unsigned int i = 0; i < numEvents; i++)
			{
				m_dialogueEvents.push(va_arg(args, DialogueEvent));
			}

			va_end(args);
		}

		inline void AddDialogueEvent(const DialogueEvent& event)
		{
			m_dialogueEvents.emplace(event);
		}

		/// <summary>
		/// Returns the next dialogue event and pops it from the dialogue queue.
		/// </summary>
		/// <returns></returns>
		inline DialogueEvent Next()
		{
			if (m_dialogueEvents.empty()) { return DialogueEvent(""); };

			DialogueEvent de = m_dialogueEvents.front();
			m_dialogueEvents.pop();
			return de;
		}

	private:
		/// <summary>
		/// A queue of all upcoming dialogue events.
		/// </summary>
		std::queue<DialogueEvent> m_dialogueEvents;
	};
}