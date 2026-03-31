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
			if (getShouldEnableOnStart()) { m_index = UpdateManager::RegisterUpdateable(this); return; }
			
			m_index = -1;
		}

		RemoveableUpdateable::~RemoveableUpdateable()
		{
			disableUpdates();
		}

		void RemoveableUpdateable::disableUpdates()
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

		void RemoveableUpdateable::enableUpdates()
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
