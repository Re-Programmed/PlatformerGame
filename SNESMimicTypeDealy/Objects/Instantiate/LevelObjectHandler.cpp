#include "LevelObjectHandler.h"
#include "../../!TestGame/TestGame.h"
#include "../../!TestGame/Mappings.h"
#include <fstream>

namespace GAME_NAME::Objects::Instantiate
{
	GameObject* LevelObjectHandler::GetLevelObject(std::string parent, std::string code, bool spawnObject, std::string levelSource)
	{
using string = std::string;

		string objectFile = AssetPath;
		objectFile += levelSource == "" ? TestGame::INSTANCE->GetCurrentLevelPath() : levelSource;
		objectFile += ObjectSubfolder;
		objectFile += "/" + parent + ".pk";

		std::ifstream objectFileStream(objectFile);
		string line;

		uint32_t currentLine = 1;

		string currentObjectHeader;

		while (std::getline(objectFileStream, line, '\n'))
		{
			if (currentLine % 2 == 1)
			{
				currentObjectHeader = line;
			}
			else if(currentObjectHeader == code)
			{
				return GAME_NAME::Mappings::LoadObjectWithDefaultMapping(line);

			}

			currentLine++;
		}

		return nullptr;
	}
	std::vector<GameObject*> LevelObjectHandler::GetLevelObjects(std::string parent)
	{
		std::vector<GameObject*> allObjects;

using string = std::string;

		string objectFile = AssetPath;
		objectFile += TestGame::GetGlobalLevelData()->GetLevelPath();
		objectFile += ObjectSubfolder;
		objectFile += "/" + parent + ".pk";

		std::ifstream objectFileStream(objectFile);
		string line;

		uint32_t currentLine = 1;

		while (std::getline(objectFileStream, line, '\n'))
		{
			if (currentLine % 2 == 0)
			{
				allObjects.push_back(GAME_NAME::Mappings::LoadObjectWithDefaultMapping(line));
			}
			
			currentLine++;
		}

		return allObjects;
	}
}

