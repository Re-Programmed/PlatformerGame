#include "SaveManager.h"
#include "../base64.h"
#include "../../Settings/AppDataFileManager.h"
#include "../../!TestGame/TestGame.h"

namespace GAME_NAME::Resources
{
	std::string SaveManager::m_currentSaveFile;
	std::string SaveManager::m_currentLevelSaveFile;

	std::shared_ptr<std::vector<std::string>> SaveManager::ReadSaveFile(std::string fileName)
	{
		const std::filesystem::path fileFolderPath = SM_AppData + std::string(APPDATA_SUBFOLDER).append(SAVE_SUBFOLDER_NAME);
		const std::filesystem::path filePath = std::string(fileFolderPath.string()).append("\\").append(m_currentSaveFile).append("\\" + fileName).append(SAVE_FILE_EXTENSION);

		std::shared_ptr<std::vector<std::string>> data(new std::vector<std::string>());

		if (!std::filesystem::exists(filePath)) { return data; }

		std::string fullFile;
		std::ifstream readFile(filePath);
		std::getline(readFile, fullFile);
		readFile.close();
		std::stringstream fileDecoded(B64::Decode(fullFile));
		std::string line;

		while (std::getline(fileDecoded, line, '\n'))
		{
			data->push_back(line);
		}

		return data;
	}

	void SaveManager::CreateSaveFile(std::string fileContents, std::string fileName)
	{
		const std::filesystem::path fileFolderPath = SM_AppData + std::string(APPDATA_SUBFOLDER).append(SAVE_SUBFOLDER_NAME);
		const std::filesystem::path filePath = std::string(fileFolderPath.string()).append("\\").append(m_currentSaveFile).append("\\" + fileName).append(SAVE_FILE_EXTENSION);

		if (!std::filesystem::exists(filePath.parent_path()))
		{
			std::filesystem::create_directories(filePath.parent_path());
		}

		std::ofstream createstream(filePath);
		createstream << B64::Encode(fileContents);
		createstream.close();
	}

	void SaveManager::SaveString(std::string data, std::string vName)
	{
		const std::filesystem::path fileFolderPath = SM_AppData + std::string(APPDATA_SUBFOLDER).append(SAVE_SUBFOLDER_NAME);

		const std::filesystem::path filePath = std::string(fileFolderPath.string()).append("\\").append(m_currentSaveFile).append(SAVE_FILE_EXTENSION);

		if (std::filesystem::exists(filePath))
		{
			std::string fullFile;

			std::ifstream readFile(filePath);

			std::getline(readFile, fullFile);

			readFile.close();

			std::stringstream fileDecoded(B64::Decode(fullFile));
			std::string decodedFile;

			std::string line;
			while (std::getline(fileDecoded, line, '\n'))
			{
				if (!line.starts_with(vName))
				{
					decodedFile.append(line + '\n');
				}
			}

			std::ofstream createstream(filePath);
			createstream << B64::Encode(decodedFile + vName + "{" + data);
			createstream.close();
		}
		else
		{
			std::filesystem::create_directories(filePath.parent_path());

			std::ofstream createstream(filePath);
			createstream << B64::Encode(vName + "{" + data);
			createstream.close();

			SaveString(data, vName);
		}

	}

	void SaveManager::GetSaveString(std::string vName, std::string& defaultValue)
	{
		const std::filesystem::path fileFolderPath = SM_AppData + std::string(APPDATA_SUBFOLDER).append(SAVE_SUBFOLDER_NAME);
		const std::filesystem::path filePath = std::string(fileFolderPath.string()).append("\\").append(m_currentSaveFile).append(SAVE_FILE_EXTENSION);
		
		if (std::filesystem::exists(filePath))
		{
			std::string fullFile;

			std::ifstream readFile(filePath);

			std::getline(readFile, fullFile);

			readFile.close();

			std::stringstream fileDecoded(B64::Decode(fullFile));

			std::string line;
			while (std::getline(fileDecoded, line, '\n'))
			{
				if (line.starts_with(vName))
				{
					defaultValue = line.replace(line.begin(), line.begin() + vName.length() + 1, "");
					defaultValue.resize(defaultValue.length());
					return;
				}
			}
		}

		SaveString(defaultValue, vName);
	}

	void SaveManager::SaveLevelString(std::string data, size_t levelID)
	{
		const std::filesystem::path fileFolderPath = SM_AppData + std::string(APPDATA_SUBFOLDER).append(SAVE_SUBFOLDER_NAME);
		const std::filesystem::path filePath = std::string(fileFolderPath.string()).append("\\").append(m_currentLevelSaveFile).append(SAVE_FILE_EXTENSION);

		if (std::filesystem::exists(filePath))
		{
			std::string finalWrite;
			std::string line;

			std::ifstream readFile(filePath);

			size_t i = 0;
			while (std::getline(readFile, line, '\n'))
			{
				if (i == levelID)
				{
					finalWrite += B64::Encode(data) + "\n";
				}
				else {
					finalWrite += line + "\n";
				}

				i++;
			}

			readFile.close();

			while (i <= levelID)
			{
				if (i == levelID)
				{
					finalWrite += B64::Encode(data) + "\n";
				}
				else {
					finalWrite += "\n";
				}
				i++;
			}

			finalWrite.erase(finalWrite.begin() + finalWrite.length() - 1);

			std::ofstream writeFile(filePath);

			writeFile << finalWrite;

			writeFile.close();
		}
		else {
			std::filesystem::create_directories(filePath.parent_path());

			std::ofstream createstream(filePath);
			createstream << B64::Encode(data);
			createstream.close();
		}
	}

	void SaveManager::GetLevelString(std::string& data, size_t levelID)
	{
		const std::filesystem::path fileFolderPath = SM_AppData + std::string(APPDATA_SUBFOLDER).append(SAVE_SUBFOLDER_NAME);
		const std::filesystem::path filePath = std::string(fileFolderPath.string()).append("\\").append(m_currentLevelSaveFile).append(SAVE_FILE_EXTENSION);

		if (std::filesystem::exists(filePath))
		{
			std::string line;

			std::ifstream readFile(filePath);

			size_t i = 0;
			while (std::getline(readFile, line, '\n'))
			{
				if (i == levelID)
				{
					data = B64::Decode(line);
					return;
				}

				i++;
			}

			readFile.close();
		}
		else {
			std::filesystem::create_directories(filePath.parent_path());

			std::ofstream createstream(filePath);
			size_t i = 0;
			while (i < levelID)
			{
				createstream << "\n";
				i++;
			}

			createstream << B64::Encode(data);

			createstream.close();
		}
	}

	void SaveManager::SetCurrentFile(std::string fileName)
	{

		m_currentSaveFile = fileName;
	}

}
