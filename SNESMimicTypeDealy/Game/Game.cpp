#include "Game.h"


#if _DEBUG
#include "../Debug/DebugLog.h"
#endif

#include "../Resources/Save/SaveManager.h"

namespace GAME_NAME
{
	namespace Game
	{


		void Game::LoadLevel(const char* levelPath, LEVEL_DATA dataToLoad, bool reload)
		{
			using namespace Resources;
			using namespace Objects;
			using namespace Levels;
			using namespace Rendering;

			if ((dataToLoad & LEVEL_DATA_AUDIO_MUSIC) != 0)
			{
				AssetManager::LoadMusic(levelPath, reload);
				AssetManager::LoadSFX(levelPath, false);
			}

			if ((dataToLoad & LEVEL_DATA_AUDIO_SFX) != 0)
			{
				/*LOAD SFX*/
			}

			if ((dataToLoad & LEVEL_DATA_TEXTURES_BACKGROUND) != 0)
			{
				AssetManager::LoadTextures(levelPath, BACKGROUND, reload);
			}

			if ((dataToLoad & LEVEL_DATA_TEXTURES_SPRITES) != 0)
			{
				AssetManager::LoadTextures(levelPath, SPRITES, reload);
			}

			if ((dataToLoad & LEVEL_DATA_DATA_CHUNKS) != 0)
			{
				auto data = AssetManager::GetChunkData(levelPath);
				Renderer::InitChunks(data);
			}
			
			if ((dataToLoad & LEVEL_DATA_DATA_LEVEL) != 0)
			{
				m_level.Path = levelPath;

				SaveManager::SetCurrentLevelFile(SaveManager::GetCurrentFile() + levelPath);

				std::string data[LevelDataSize];
				AssetManager::GetLevelData(levelPath, data);

				LevelDataParser* ldp = new LevelDataParser(data, m_level);

				delete ldp;

#if _DEBUG
				DEBUG::DebugLog::Log("Loaded Level: " + m_level.Name, true, ";39;41;1;4");
				std::string start = "ID: " + std::to_string(m_level.ID.World);
				DEBUG::DebugLog::Log(start + "-" + std::to_string(m_level.ID.Level), true, ";39;41;1;4");
#endif
				InitLevel(m_level);
			}

		}

		void Game::ClearLevel(LEVEL_DATA data)
		{
			using namespace Resources;
			using namespace Objects;
			using namespace Levels;
			using namespace Rendering;

			if ((data & LEVEL_DATA_AUDIO_MUSIC) != 0)
			{
				Audio::SoundManager::ClearSources();
			}else if ((data & LEVEL_DATA_AUDIO_SFX) != 0)
			{
				Audio::SoundManager::ClearSources();
			}

			if ((data & LEVEL_DATA_TEXTURES_BACKGROUND) != 0)
			{
				Rendering::Renderer::ClearTextures();
				return;
			}

			if ((data & LEVEL_DATA_TEXTURES_SPRITES) != 0)
			{
				Rendering::Renderer::ClearTextures();
			}
		}


		Rendering::Camera::Camera* Game::GetCamera()
		{
			return m_camera;
		}
	}
}
