#pragma once

#include "SoundManager.h"

#include "../Utils/Math/Vec2.h"

#include <array>
#include <tuple>

namespace GAME_NAME::Audio
{
	using namespace MathUtils;

	class SoundEvents
	{
	public:
		/// <summary>
		/// Allows for specific groups of sounds to change volume seperatley.
		/// </summary>
		enum class SoundControlGroup
		{
			Music,
			SFX
		};
		
#define SOUND_EVENT_COUNT 15
		enum class Event
		{
			PLAYER_BIG_FALL,
			PLAYER_SKID,
			ATTACK_SWIPE,
			OBJECT_LAND_GRASS,
			OBJECT_WALK_GRASS,
			OBJECT_LAND_WOOD,
			OBJECT_WALK_WOOD,
			DIALOGUE_SPEAKING,
			DIALOGUE_SPEAKING_LOW,
			MONEY,
			GUI_SELECT,
			GUI_WRITE,
			GUI_ITEM,
			HIT_PUNCHED,
			HIT_SLICED
		};

		/// <summary>
		/// Adjusts the volume and panning based on where a sound is placed from the center of the screen.
		/// </summary>
		/// <param name="soundEvent">[SoundEvents::Event] - what sound to play.</param>
		/// <param name="group">[SoundControlGroup] - what "fader" controls this sound.</param>
		/// <param name="position">[Vec2] - where the sound originates.</param>
		/// <param name="volumeScale">[float] - multiplied by the volume.</param>
		static AudioVoice PlaySoundAtPoint(Event soundEvent, Vec2 position, float volumeScale = 1.f, bool paused = false);

		static AudioVoice PlaySoundGlobal(Event soundEvent, float volumeScale = 1.f, float panning = 0.f, bool paused = false);

	private:
		static std::array<std::tuple<std::string, SoundControlGroup>, SOUND_EVENT_COUNT> m_soundEventKeys;

		static inline std::tuple<std::string, SoundControlGroup>& getSoundEventData(Event event)
		{
			unsigned int index = static_cast<unsigned int>(event);
			if (index >= SOUND_EVENT_COUNT)
			{
				return m_soundEventKeys.at(0);
			}

			return m_soundEventKeys.at(index);
		}

		/// <summary>
		/// Get the position of the center of the screen.
		/// </summary>
		/// <returns></returns>
		static Vec2 getHeadphonePosition();

		/// <summary>
		/// Returns the audio bus and group for the specified control group.
		/// </summary>
		/// <param name="group"></param>
		/// <returns></returns>
		static std::tuple<AudioGroup&, AudioBus&> getControlGroup(SoundControlGroup group);
	};
}