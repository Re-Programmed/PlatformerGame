#pragma once

#include <string>

namespace GAME_NAME
{

	namespace Objects
	{
		/// <summary>
		/// GameObjects that also inherit this class are objects that should be considered part of the save files for a player and can have data loaded and stored on game load and game save.
		/// </summary>
		class GameObjectState
		{
		public:
			/// <summary>
			/// Create a new GameObjectState, the ID is the index that will be used for storing the objects state in a file.
			/// </summary>
			GameObjectState(size_t objectSaveID)
				: m_objectSaveID(objectSaveID)
			{
				//LoadState();
			}

			/// <summary>
			/// Loads this objects state from the current save file.
			/// </summary>
			virtual void LoadState() = 0;
			
			/// <summary>
			/// Saves this objects state to the current save file.
			/// </summary>
			virtual void SaveState() = 0;

			/// <summary>
			/// Set weather this object is ready to be saved. Useful for checking if an object has been modified and only registering objects to be saved once.
			/// </summary>
			/// <param name="toBeSaved"></param>
			inline void SetToBeSaved(bool toBeSaved)
			{
				m_toBeSaved = toBeSaved;
			}

			inline const size_t& GetObjectSaveId()
			{
				return m_objectSaveID;
			}
		protected:
			/// <summary>
			/// Object save ID, should be set on object creation and never changed.
			/// </summary>
			const size_t m_objectSaveID;
			/// <summary>
			/// Weather this object will be saved on the next game save.
			/// </summary>
			bool m_toBeSaved = false;
		};
	}
}