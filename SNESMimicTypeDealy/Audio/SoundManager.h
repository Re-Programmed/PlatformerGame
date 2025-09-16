#pragma once
#include <soloud.h>
#include <vector>
#include <memory>
#include <map>
#include <string>

namespace GAME_NAME
{
	namespace Audio
	{
		constexpr uint8_t UnderwaterMusicID = 1;

		typedef SoLoud::Soloud AudioPlayer;
		typedef SoLoud::handle AudioGroup;
		typedef SoLoud::Bus AudioBus;
		typedef SoLoud::handle AudioVoice;
		typedef SoLoud::AudioSource AudioSource;
		typedef SoLoud::time TimeSeconds;

		/*static*/ class SoundManager
		{
		public:
#pragma region MusicData
			static AudioVoice BGMusic;		//The current background music.	(Should be stopped before updating.)
			static AudioVoice WaterMusic;	//The current background underwater music.	(Should be stopped before updating.)
#pragma endregion

			static AudioGroup MusicGroup;	//The audio group for all music. (Used to change the volume in settings menu.)
			static AudioGroup SFXGroup;		//The audio group for all sound effects. (Used to change the volume in settings menu.)

			static AudioBus MusicBus;	//The audio group for all music. (Used to change the volume in settings menu.)
			static AudioBus SFXBus;		//The audio group for all sound effects. (Used to change the volume in settings menu.)

			static void SaveAllAudioBusVolumes();


			static void Init();				//Initilizes the AudioPlayer.
			static void DeInit();			//Destroys the AudioPlayer (No memory leaks :]).

			static AudioPlayer* GetAudioPlayer()	//Returns the audio player which can be used to perform certain operations on AudioVoices.
			{
				return m_audioPlayer;
			};

			static void RegisterAudioSource(std::string key, AudioSource* source);	//Registers an audio source to an array of sources that can be loaded into voices and played.

			static std::unique_ptr<AudioSource>& GetAudioSource(std::string key);

			static void ClearSources();												//Removes all audio sources. (Can be called when a new level loads and new sources must be loaded.)

			static AudioVoice Play(std::string key, AudioGroup group, float volume = -1.0F, float pan = 0.0F, bool paused = false, unsigned int bus = 0U);	//Plays the audio source of the specified key and returns the AudioVoice it is loaded to.
		private: 
			static std::map<std::string, std::unique_ptr<AudioSource>> m_sources;	//Stores all the currently available audio sources.

			static AudioPlayer* m_audioPlayer;										//The player.
		};
	}
}