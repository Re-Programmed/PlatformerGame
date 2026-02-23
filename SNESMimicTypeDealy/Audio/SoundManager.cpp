#include "SoundManager.h"
#if _DEBUG
#include "../Debug/DebugLog.h"
#endif

#include "../EngineCompileOptions.h"

#define AUDIO_SETTINGS_FOLDER "settings\\audio.dat"

#include "../Settings/AppDataFileManager.h"

namespace GAME_NAME
{
	namespace Audio
	{
		AudioPlayer* SoundManager::m_audioPlayer;
		AudioGroup SoundManager::MusicGroup;
		AudioBus SoundManager::MusicBus;
#define SOUND_MANAGER_MUSIC_BUS_DATA_NAME "Music"
		AudioGroup SoundManager::SFXGroup;
		AudioBus SoundManager::SFXBus;
#define SOUND_MANAGER_SFX_BUS_DATA_NAME "SFX"
		


		std::map<std::string, std::unique_ptr<AudioSource>> SoundManager::m_sources;

		AudioVoice SoundManager::BGMusic;
		AudioVoice SoundManager::WaterMusic;

		void SoundManager::SaveAllAudioBusVolumes()
		{
			AppData::AppDataFileManager::SetValue(AUDIO_SETTINGS_FOLDER, SOUND_MANAGER_MUSIC_BUS_DATA_NAME, std::to_string(MusicBus.mVolume));
			AppData::AppDataFileManager::SetValue(AUDIO_SETTINGS_FOLDER, SOUND_MANAGER_SFX_BUS_DATA_NAME, std::to_string(SFXBus.mVolume));
		}

		void SoundManager::Init()
		{
			m_audioPlayer = new AudioPlayer();

#if _DEBUG
			DEBUG::DebugLog::Log("[MUSIC] Initilized MusicManager.");
#endif
			m_audioPlayer->init();

			SFXGroup = m_audioPlayer->createVoiceGroup();
			MusicGroup = m_audioPlayer->createVoiceGroup();

			float musicBusVolume = std::stof(AppData::AppDataFileManager::GetValue(AUDIO_SETTINGS_FOLDER, SOUND_MANAGER_MUSIC_BUS_DATA_NAME, "1"));
			MusicBus.setVolume(musicBusVolume);
			float SFXBusVolume = std::stof(AppData::AppDataFileManager::GetValue(AUDIO_SETTINGS_FOLDER, SOUND_MANAGER_SFX_BUS_DATA_NAME, "1"));
			SFXBus.setVolume(SFXBusVolume);
		}

		void SoundManager::DeInit()
		{
#if _DEBUG
			DEBUG::DebugLog::Log("[MUSIC] De-Initilized MusicManager.");
#endif
			m_audioPlayer->deinit();
			//delete m_audioPlayer;
		}


		

		void SoundManager::RegisterAudioSource(std::string key, AudioSource* source)
		{
			m_sources.insert( std::pair<std::string, std::unique_ptr<AudioSource>>(key, std::unique_ptr<AudioSource>(source)) );
		}

		std::unique_ptr<AudioSource>& SoundManager::GetAudioSource(std::string key)
		{
			if (m_sources.count(key) <= 0)
			{
#ifdef SHOW_WINDOWS_ERROR_POPUPS
				std::string message = "Tried to get a non-existant sound: \"";
				message.append(key).append("\"");
				std::wstring stemp = std::wstring(message.begin(), message.end());
				MessageBox(nullptr, stemp.c_str(), TEXT("Resource Error"), MB_OK);
#endif
			}

			return m_sources.at(key);
		}

		void SoundManager::ClearSources()
		{
			for (const auto& pair : m_sources)
			{
				pair.second->stop();
			}

			m_sources.clear();
		}

		AudioVoice SoundManager::Play(std::string key, AudioGroup group, float volume, float pan, bool paused, unsigned int bus)
		{
			AudioVoice aVoice = m_audioPlayer->play(*GetAudioSource(key).get(), volume, pan, true, bus);
			m_audioPlayer->addVoiceToGroup(group, aVoice);

			if (!paused)
			{
				m_audioPlayer->setPause(aVoice, false);
			}

			return aVoice;
		}

	}
}