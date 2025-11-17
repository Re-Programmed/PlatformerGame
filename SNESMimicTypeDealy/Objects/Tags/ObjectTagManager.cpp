#include "ObjectTagManager.h"

#include "../../EngineCompileOptions.h"

#ifdef SHOW_WINDOWS_ERROR_POPUPS
#include <windows.h>
#endif

namespace GAME_NAME::Objects::Tags
{
	std::multimap<std::string, GameObject*> ObjectTagManager::m_taggedObjects;
	std::mutex ObjectTagManager::m_taggedObjectsLock;

	std::pair<std::multimap<std::string, GameObject*>::const_iterator, std::multimap<std::string, GameObject*>::const_iterator> ObjectTagManager::GetObjectsWithTag(std::string tag)
	{

		if (!m_taggedObjects.contains(tag))
		{
#ifdef SHOW_WINDOWS_ERROR_POPUPS
			std::string message = "Failed to find an object tag: \"" + tag + "\"";
			std::wstring stemp = std::wstring(message.begin(), message.end());
			MessageBox(nullptr, stemp.c_str(), TEXT("Resource Error"), MB_OK);
#endif
		}

		return m_taggedObjects.equal_range(tag);
	}

	GameObject* ObjectTagManager::GetObjectWithTag(std::string tag)
	{
		auto[begin, end] = GetObjectsWithTag(tag);

		if (begin != end)
		{
			return begin->second;
		}


		return nullptr;
	}

}