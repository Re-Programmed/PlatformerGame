#pragma once

#include "../Game/Game.h"
#include "../Game/Level.h"
#include "./Objects/Player/Player.h"
#include "./Camera/GameCamera.h"
#include "Level/GlobalLevelData.h"
#include "Level/LevelSystem.h"

namespace GAME_NAME
{
	using namespace Objects;
	using namespace Player;
	using namespace Level;

	class TestGame : public Game::Game
	{
	public:
		TestGame()
			: m_gameCamera(nullptr)
		{

		}

		static std::shared_ptr<Objects::Player::Player> ThePlayer;	//Stores a reference to the player object.

		static TestGame* INSTANCE;									//Singleton.

		void Update(GLFWwindow* window) override;					//Called every frame before rendering.
		void Init(GLFWwindow* window) override;						//Called when the program loads.
		void LateUpdate(GLFWwindow* window) override;				//Called every frame after rendering.

		void InitLevel(GAME_NAME::Game::Level level);				//Called when a new level loads.

		static GLFWwindow* FirstWindow;

		/// <summary>
		/// Returns true if the game is paused.
		/// </summary>
		inline static bool GetGamePaused()
		{
			return m_gamePaused;
		}

		static void TogglePauseState();

		/// <summary>
		/// Retuns a pointer to the current level's global level data.
		/// </summary>
		/// <returns></returns>
		static inline GlobalLevelData* GetGlobalLevelData()
		{
			return m_globalLevelData;
		}

		/// <summary>
		/// Deletes the old camera and sets the current game camera to the provided newCam.
		/// </summary>
		inline void SetCamera(GAME_NAME::Camera::GameCamera* newCam)
		{
			delete m_gameCamera;
			m_gameCamera = newCam;
		}

		static inline const LevelSystem* GetCurrentLevelSystem()
		{
			return m_currentLevelSystem.get();
		}

		inline std::string GetCurrentLevelPath()
		{
			return m_level.Path;
		}

		/// <summary>
		/// The basic level loading function. Loads everything with global_assets in order.
		/// </summary>
		/// <param name="levelPath"></param>
		void LoadLevelAndAllData(const char* levelPath);
		void LoadLevelOnlyObjects(const char* levelPath, Vec2 newPlayerPos);

		static inline void SetLoadLevelWithSavedPlayer(bool value) { m_loadLevelWithSavedPlayer = value; }

	private:
		static bool m_loadLevelWithSavedPlayer;

		static std::unique_ptr<LevelSystem> m_currentLevelSystem;

		GAME_NAME::Camera::GameCamera* m_gameCamera = nullptr;		//Pointer to the camera.

		/// <summary>
		/// Global Level Data for the current level.
		/// </summary>
		static GlobalLevelData* m_globalLevelData;
		
		/// <summary>
		/// True if the game is paused.
		/// </summary>
		static bool m_gamePaused;

		/// <summary>
		/// Stores the objects created to represent the pause menu and to delete them after the menu is closed.
		/// </summary>
		static std::vector<GUI::StaticGUIElement*> m_pauseMenuObjects;
	};
}