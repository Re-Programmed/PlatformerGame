#pragma once

#include "IUpdateable.h" 
namespace GAME_NAME
{
	namespace Utils
	{
		/// <summary>
		/// An updateable that can be removed from getting updated at any time. 
		/// A RemoveableUpdateable gets automatically added and removed from UpdateManager
		/// when it is instantiated or destroyed respectivley.
		/// </summary>
		class RemoveableUpdateable : public IUpdateable
		{
		public:
			RemoveableUpdateable();
			~RemoveableUpdateable();

		protected:
			/// <summary>
			/// Stops the updateable from getting updated without destroying it. [UPDATES MUST BE ENABLED TO CALL]
			/// </summary>
			void disableUpdates();

			/// <summary>
			/// Reenables the updateable getting updates. [UPDATES MUST BE DISABLED TO CALL]
			/// </summary>
			void enableUpdates();

			virtual bool getShouldEnableOnStart()
			{
				return true;
			}
		private:
			UpdateIndex m_index;
		};
	}
}