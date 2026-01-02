#include "TestGame.h"
#include "../Utils/RemoveableUpdateable.h"
namespace GAME_NAME
{
	class DeathEventManager
		: public RemoveableUpdateable
	{
	public:

		static void ReloadFromSave();

		enum DEATH_SCREEN_TYPE
		{
			FALLEN,
			KILLED_BY_ENEMY
		};

		static void ShowDeathScreen(DEATH_SCREEN_TYPE screenType);

		void Update(GLFWwindow* window) override;

	protected:
		inline bool GetShouldEnableOnStart() override { return true; }

	private:
		static std::unique_ptr<DeathEventManager> UPDATE_INSTANCE;


		static void deathButtonEvents(int buttonID);
		static unsigned int m_retryButtonID;

		/// <summary>
		/// Stores where items that save the player's position have been used.
		/// </summary>
		static Vec2 m_playerFlaggedPosition;
	};
}