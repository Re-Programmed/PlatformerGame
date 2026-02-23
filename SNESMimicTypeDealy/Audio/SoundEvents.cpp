#include "SoundEvents.h"

#include "../!TestGame/TestGame.h"

namespace GAME_NAME::Audio
{
	std::array<std::tuple<std::string, SoundEvents::SoundControlGroup>, SOUND_EVENT_COUNT> SoundEvents::m_soundEventKeys = {
		std::tuple("big_fall",  SoundEvents::SoundControlGroup::SFX),
		std::tuple("walk_skidding",  SoundEvents::SoundControlGroup::SFX),
		std::tuple("attack_swipe",  SoundEvents::SoundControlGroup::SFX),
		std::tuple("land_grass",  SoundEvents::SoundControlGroup::SFX),
		std::tuple("walk_grass",  SoundEvents::SoundControlGroup::SFX),
		std::tuple("land_wood",  SoundEvents::SoundControlGroup::SFX),
		std::tuple("walk_wood",  SoundEvents::SoundControlGroup::SFX),
		std::tuple("dialogue_speaking",  SoundEvents::SoundControlGroup::SFX),
		std::tuple("dialogue_speaking_low",  SoundEvents::SoundControlGroup::SFX),
		std::tuple("money",  SoundEvents::SoundControlGroup::SFX),
		std::tuple("gui_select",  SoundEvents::SoundControlGroup::SFX),
		std::tuple("gui_write",  SoundEvents::SoundControlGroup::SFX),
		std::tuple("gui_item",  SoundEvents::SoundControlGroup::SFX),
		std::tuple("hit_punched",  SoundEvents::SoundControlGroup::SFX),
		std::tuple("hit_sliced",  SoundEvents::SoundControlGroup::SFX)
	};

	AudioVoice SoundEvents::PlaySoundAtPoint(Event soundEvent, Vec2 position, float volumeScale, bool paused)
	{
		const Vec2 headphonePosition = getHeadphonePosition();
		float percentageOff = std::abs(position.X - headphonePosition.X) / TargetResolutionX;
		float volume = volumeScale * (1.f - (percentageOff));
		volume = std::clamp(volume, 0.f, 1.f);

		float panning = percentageOff * (position.X < headphonePosition.X ? -1.f : 1.f);
		panning = std::clamp(panning, -1.f, 1.f);

		std::tuple<std::string, SoundControlGroup>& soundEventData = getSoundEventData(soundEvent);
		std::tuple<AudioGroup&, AudioBus&> controlGroup = getControlGroup(std::get<SoundControlGroup>(soundEventData));
		return SoundManager::Play(std::get<std::string>(soundEventData), std::get<AudioGroup&>(controlGroup), volume, panning, paused, std::get<AudioBus&>(controlGroup).mChannelHandle);
	}

	AudioVoice SoundEvents::PlaySoundGlobal(Event soundEvent, float volumeScale, float panning, bool paused)
	{
		std::tuple<std::string, SoundControlGroup>& soundEventData = getSoundEventData(soundEvent);
		std::tuple<AudioGroup&, AudioBus&> controlGroup = getControlGroup(std::get<SoundControlGroup>(soundEventData));
		return SoundManager::Play(std::get<std::string>(soundEventData), std::get<AudioGroup&>(controlGroup), volumeScale, panning, paused, std::get<AudioBus&>(controlGroup).mChannelHandle);
	}

	Vec2 SoundEvents::getHeadphonePosition()
	{
		return TestGame::INSTANCE->GetCamera()->GetPosition() + Vec2{ TargetResolutionX / 2.f, TargetResolutionY / 2.f };
	}

	std::tuple<AudioGroup&, AudioBus&> SoundEvents::getControlGroup(SoundControlGroup group)
	{
		switch (group)
		{
		case SoundControlGroup::Music:
			return std::tuple<AudioGroup&, AudioBus&>(SoundManager::MusicGroup, SoundManager::MusicBus);
		case SoundControlGroup::SFX:
		default:
			return std::tuple<AudioGroup&, AudioBus&>(SoundManager::SFXGroup, SoundManager::SFXBus);
		}
	}

}
