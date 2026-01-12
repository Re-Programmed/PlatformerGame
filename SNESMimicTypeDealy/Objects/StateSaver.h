#pragma once

#include <vector>
#include "./MiscStateGroup.h"
#include "./GameObjectState.h"
#include <memory>
#include "../Resources/Save/SaveManager.h"


namespace GAME_NAME::Objects
{
	/// <summary>
	/// Used to save object states.
	/// </summary>
	class StateSaver
	{
	public:
		/// <summary>
		/// Registers a GameObjectState to be saved on the next save.
		/// The object will no longer be registed after it is saved and should be assigned to be saved after an update to the object occurs.
		/// </summary>
		/// <param name="state"></param>
		static void RegisterToBeSaved(GameObjectState* state);

		/// <summary>
		/// Registers a MiscStateGroup to be saved any time the game is saved.
		/// The state group will always be saved from then on (unlike GameObjectStates)
		/// </summary>
		/// <param name="state"></param>
		static void RegisterToBeSaved(MiscStateGroup* state);
		static void UnregisterToBeSaved(MiscStateGroup* state);

		/// <summary>
		/// Saves all currently registered GameObjectStates and clears the cache.
		/// </summary>
		static void SaveStates();

		/// <summary>
		/// Saves all misc states.
		/// </summary>
		static void SaveMisc();
	private:
		/// <summary>
		/// A list of all the objects that have been modified and, therefore, should be saved on the next save.
		/// </summary>
		static std::vector<GameObjectState*> m_toBeSaved;

		static std::vector<MiscStateGroup*> m_miscStates;

	};
}