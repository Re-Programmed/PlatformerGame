#include "UpdateManager.h"
#include "Time/GameTime.h"
#include "../Components/Physics/Collision/CollisionManager.h"


namespace GAME_NAME
{
	namespace Utils
	{
		std::vector<IUpdateable*> UpdateManager::m_updateables{
		};


		void UpdateManager::Update(GLFWwindow* window)
		{
			//Use const index because sometimes updatables create other updatables while they are updating! Yay!
			size_t updateableSize = m_updateables.size();
			for (size_t i = 0; i < updateableSize; i++)
			{
				if (updateableSize > m_updateables.size())
				{
					updateableSize = m_updateables.size();
				}

				IUpdateable*& u = m_updateables[i];
				u->Update(window);
			}

		}

		UpdateIndex UpdateManager::RegisterUpdateable(IUpdateable* updateable)
		{
			m_updateables.push_back(updateable);
			return (Utils::UpdateIndex)m_updateables.size() - 1;
		}

		void UpdateManager::RemoveUpdateable(const UpdateIndex index)
		{
			m_updateables.erase(m_updateables.begin() + index);
		}
	}
}