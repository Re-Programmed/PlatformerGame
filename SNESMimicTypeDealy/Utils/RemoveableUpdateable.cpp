#include "RemoveableUpdateable.h"
#include "UpdateManager.h"
#if _DEBUG
#include <iostream>
#endif

namespace GAME_NAME
{
	namespace Utils
	{
		RemoveableUpdateable::RemoveableUpdateable()
		{
			if (GetShouldEnableOnStart()) { m_index = UpdateManager::RegisterUpdateable(this); return; }
			
			m_index = -1;
		}

		RemoveableUpdateable::~RemoveableUpdateable()
		{
			DisableUpdates();
		}

		void RemoveableUpdateable::DisableUpdates()
		{
#if _DEBUG
			if (m_index == -1)
			{
				std::cout << "Tried to disable updates on an object that already has no updates." << std::endl;
				return;
			}
#endif
			UpdateManager::RemoveUpdateable(m_index);
			m_index = -1;
		}

		void RemoveableUpdateable::EnableUpdates()
		{
#if _DEBUG
			if (m_index != -1)
			{
				std::cout << "Tried to enable updates on an object that already has updates." << std::endl;
				return;
			}
#endif

			m_index = UpdateManager::RegisterUpdateable(this);
		}

		
	}
}
