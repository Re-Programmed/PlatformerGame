#pragma once
#include "../../Utils/Math/Vec2.h"
#include <memory>

#define START_STATE_FOLDER "start_state"

namespace GAME_NAME::Objects
{
	using namespace MathUtils;

	/// <summary>
	/// A StartState can be created to save the level at a specific moment (for example crafting a save point item) so that death or restarting will take you to this point.
	/// The standard save data system is utilized when moving between levels and other conditions, so using the StartState system will avoid these save events.
	/// </summary>
	class StartState
	{
	public:
		/// <summary>
		/// Copys all current save data to a new folder for the StartState.
		/// </summary>
		static void CreateStartState();

		/// <summary>
		/// The next data loaded will be from the start state.
		/// </summary>
		/// <returns></returns>
		static bool SetToBeLoaded();

		/// <summary>
		/// Called to reset the save system to use the normal save data after a StartState save has been loaded.
		/// </summary>
		static void FinishLoad();
	private:
		/// <summary>
		/// Used to remember what data was copied.
		/// </summary>
		static std::string m_lastSaveFile;

	};
}