#pragma once

#include "../GameObject.h"
#include <map>
#include <mutex>

namespace GAME_NAME::Objects::Tags
{
	class ObjectTagManager
	{
	public:
		static std::pair<std::multimap<std::string, GameObject*>::const_iterator, std::multimap<std::string, GameObject*>::const_iterator> GetObjectsWithTag(std::string tag);

		/// <summary>
		/// If only one object exists with this tag, this function will return a pointer to that one object.
		/// </summary>
		/// <param name="tag"></param>
		/// <returns></returns>
		static GameObject* GetObjectWithTag(std::string tag);

		static inline void TagObject(GameObject* object, std::string tag)
		{
			m_taggedObjectsLock.lock();
			m_taggedObjects.insert(std::pair<std::string, GameObject*>(tag, object));
			m_taggedObjectsLock.unlock();
		}

		static inline void ClearTaggedObjects()
		{
			m_taggedObjectsLock.lock();
			if (m_taggedObjects.size() > 0)
			{
				m_taggedObjects.clear();
			}
			m_taggedObjectsLock.unlock();
		}

	private:
		static std::mutex m_taggedObjectsLock;
		static std::multimap<std::string, GameObject*> m_taggedObjects;
	};
}