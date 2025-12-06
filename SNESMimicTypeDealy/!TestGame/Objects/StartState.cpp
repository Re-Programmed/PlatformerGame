#include "StartState.h"
#include "../../Resources/Save/SaveManager.h"
#include "../../Settings/AppDataFileManager.h"
#include "../../Objects/StateSaver.h"

namespace GAME_NAME::Objects
{
	using namespace Resources;

	std::string StartState::m_lastSaveFile = "";

	void StartState::CreateStartState()
	{
		const std::filesystem::path fileFolderPath = SM_AppData + std::string(APPDATA_SUBFOLDER).append(SAVE_SUBFOLDER_NAME);
		const std::filesystem::path filePath = std::string(fileFolderPath.string()).append("\\").append(SaveManager::GetCurrentFile());
	
		const std::filesystem::path destination = std::string(fileFolderPath.string()).append("\\").append(START_STATE_FOLDER).append("_").append(SaveManager::GetCurrentFile());

		//Remove existing StartState if it is there.
		if (std::filesystem::exists(destination))
		{
			std::filesystem::remove_all(destination);
		}

		if (std::filesystem::exists(destination.string().append(SAVE_FILE_EXTENSION)))
		{
			std::filesystem::remove(destination.string().append(SAVE_FILE_EXTENSION));
		}

		std::filesystem::copy(filePath, destination);
		std::filesystem::copy(filePath.string().append(SAVE_FILE_EXTENSION), destination.string().append(SAVE_FILE_EXTENSION));
	}

	bool StartState::SetToBeLoaded()
	{
		if (!m_lastSaveFile.empty())
		{
			//Somehow two things are trying to load at once.
			return false;
		}

		const std::filesystem::path fileFolderPath = SM_AppData + std::string(APPDATA_SUBFOLDER).append(SAVE_SUBFOLDER_NAME);
		const std::filesystem::path destination = std::string(fileFolderPath.string()).append("\\").append(START_STATE_FOLDER).append("_").append(SaveManager::GetCurrentFile());
		if (!std::filesystem::exists(destination))
		{
			//No state has been created yet.
			return false;
		}


		m_lastSaveFile = SaveManager::GetCurrentFile();

		SaveManager::SetCurrentFile(std::string(START_STATE_FOLDER).append("_").append(m_lastSaveFile));
		return true;
	}

	void StartState::FinishLoad()
	{
		if (m_lastSaveFile.empty()) { return; }

		SaveManager::SetCurrentFile(m_lastSaveFile);
		m_lastSaveFile = "";

		//Save everything to ensure that all save data is syncronized with the data that was just loaded.
		//This will copy the StartState basck to being normal save data.
		const std::filesystem::path fileFolderPath = SM_AppData + std::string(APPDATA_SUBFOLDER).append(SAVE_SUBFOLDER_NAME);
		const std::filesystem::path filePathDest = std::string(fileFolderPath.string()).append("\\").append(SaveManager::GetCurrentFile());

		const std::filesystem::path source = std::string(fileFolderPath.string()).append("\\").append(START_STATE_FOLDER).append("_").append(SaveManager::GetCurrentFile());

		if (std::filesystem::exists(filePathDest))
		{
			std::filesystem::remove_all(filePathDest);
		}

		if (std::filesystem::exists(filePathDest.string().append(SAVE_FILE_EXTENSION)))
		{
			std::filesystem::remove_all(filePathDest.string().append(SAVE_FILE_EXTENSION));
		}


		std::filesystem::copy(source, filePathDest);
		std::filesystem::copy(source.string().append(SAVE_FILE_EXTENSION), filePathDest.string().append(SAVE_FILE_EXTENSION));
	} 

}
 
