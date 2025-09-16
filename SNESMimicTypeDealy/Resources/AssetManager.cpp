#include "AssetManager.h"
#include <string>
#include <fstream>
#include "../Audio/SoundManager.h"
#include "../Audio/SoundTypes/Sound.h"
#include "../Settings/SettingsGlobals.h"
#include <regex>
//regex for std::regex_replace
#include "../!TestGame/Mappings.h"
#include "../Objects/Tags/ObjectTagManager.h"

#include <array>

#if _DEBUG
#include "../Debug/DebugLog.h"

#define DebugHeader(x, y) { std::string s = x; DEBUG::DebugLog::LogPlain("------------------" + s + "------------------", y); }
#endif

#define LOAD_LAST_PREFIX "_m_"

namespace GAME_NAME
{

	namespace Resources
	{
		inline void CreateResoruceErrorMessage(std::string message)
		{
#ifdef SHOW_WINDOWS_ERROR_POPUPS
			std::wstring stemp = std::wstring(message.begin(), message.end());
			MessageBox(nullptr, stemp.c_str(), TEXT("Resource Error"), MB_OK);	
#endif
		}

		std::vector<std::string> AssetManager::m_loadAtEnd;
		volatile std::atomic<int> AssetManager::m_currentThreadLoadCount;
		volatile int AssetManager::m_currentThreadLoadCountINT = 0;

		void AssetManager::LoadTextures(const char* subfolder, TEXTURE_LOAD textureLoad, bool reloadTextures)
		{
#if _DEBUG
			DebugHeader("Textures", textureLoad == BACKGROUND ? ";34" : ";36");
#endif
			std::string filePath = AssetPath;
			filePath += subfolder;
			
			if (reloadTextures) { Rendering::Renderer::ClearTextures(); }

			if (textureLoad == 0)
			{
				LoadTextures(subfolder, BACKGROUND);
				filePath += SpriteSubfolder;
			}
			else {
				filePath += (textureLoad == SPRITES ? SpriteSubfolder : BGSubfolder);
			}

			if (!std::filesystem::exists(filePath))
			{
				std::string message = "Missing directory: \"" + filePath + "\"";
				CreateResoruceErrorMessage(message);
				return;
			}

			unsigned int i = 0;
			using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
			
			for (const auto& file : recursive_directory_iterator(filePath))
			{
				if (file.is_regular_file())
				{
#if _DEBUG
					GLuint myLoaded = 0;
#endif
					if (textureLoad == SPRITES || textureLoad == ALL_TEXTURES)
					{
						i++;
#if _DEBUG
						myLoaded = Rendering::Renderer::LoadSprite(file.path().string().c_str());
#else
						Rendering::Renderer::LoadSprite(file.path().string().c_str());
#endif
					}
					else {
#if _DEBUG
						myLoaded = Rendering::Renderer::LoadBG(file.path().string().c_str());
#else
						Rendering::Renderer::LoadBG(file.path().string().c_str());
#endif
					}

#if _DEBUG
					DEBUG::DebugLog::Log(std::to_string(myLoaded) + ":: " + file.path().string(), false, ";34;2");
#endif
				}
			}

			if (textureLoad == SPRITES || textureLoad == ALL_TEXTURES)
			{
				//Assign the first set of loaded sprites to be the fileOffset. (Negative sprite indicies)
				if (Renderer::GetLastFileOffest() == -1)
				{
					Renderer::SetLastFileOffset(0);
				}
				else if(Renderer::GetLastFileOffest() == 0)
				{
					Renderer::SetLastFileOffset(Renderer::GetSpriteCount() - i);
				}
			}
#if _DEBUG
			DebugHeader("", ";34");
#endif
		}

		void AssetManager::LoadMusic(const char* subfolder, bool reloadAllSounds)
		{
#if _DEBUG
			DebugHeader("Music", ";2;33");
#endif
			std::string filePath = AssetPath;
			filePath += subfolder;
			filePath += MusicFilePath;

			if (reloadAllSounds) { Audio::SoundManager::ClearSources(); }

			if (!std::filesystem::exists(filePath))
			{
				std::string message = "Missing directory: \"" + filePath + "\"";
				CreateResoruceErrorMessage(message);
				return;
			}

			using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

			for (const auto& file : recursive_directory_iterator(filePath))
			{
				if (file.is_regular_file())
				{
					Audio::SoundManager::RegisterAudioSource(file.path().stem().string(), new GAME_NAME::Audio::Sound(file.path().string().c_str()));
#if _DEBUG
					DEBUG::DebugLog::Log(file.path().string(), false, ";33;2");
#endif
				}
			}

		}

		void AssetManager::LoadSFX(const char* subfolder, bool reloadAllSounds)
		{
#if _DEBUG
			DebugHeader("SFX", ";2;33");
#endif

			std::string filePath = AssetPath;
			filePath += subfolder;
			filePath += SFXFilePath;

			if (reloadAllSounds) { Audio::SoundManager::ClearSources(); }

			if (!std::filesystem::exists(filePath))
			{
				std::string message = "Missing directory: \"" + filePath + "\"";
				CreateResoruceErrorMessage(message);
				return;
			}


			using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

			for (const auto& file : recursive_directory_iterator(filePath))
			{
				if (file.is_regular_file())
				{
					Audio::SoundManager::RegisterAudioSource(file.path().stem().string(), new GAME_NAME::Audio::Sound(file.path().string().c_str()));
#if _DEBUG
					DEBUG::DebugLog::Log(file.path().string(), false, ";33;2");
#endif
				}
			}
		}

		void AssetManager::GetLevelData(const char* subfolder, std::string data[Objects::Levels::LevelDataSize])
		{
			std::string filePath = AssetPath;
			filePath += subfolder;
			filePath += LevelFileName;

			if (!std::filesystem::exists(filePath))
			{
				std::string message = "Missing level data file: \"" + filePath + "\"";
				CreateResoruceErrorMessage(message);
				return;
			}

			std::ifstream LevelFile(filePath);

			int8_t i = 0;
			std::string text;
			while (std::getline(LevelFile, text, '\n'))
			{
				data[i] = text;
				i++;
			}

		}

		std::unordered_map<std::string, std::string> AssetManager::GetDialogueData(const char* subfolder)
		{
			std::unordered_map<std::string, std::string> data;

			std::string filePath = AssetPath;
			filePath += subfolder;
			filePath += DialogueFile;

			if (!std::filesystem::exists(filePath))
			{
				std::string message = "Missing dialogue file: \"" + filePath + "\"";
				CreateResoruceErrorMessage(message);
				return data;
			}

			std::ifstream DialogueFile(filePath);
			std::string line;

			std::string key = "", info = "";

			while (std::getline(DialogueFile, line, '\n'))
			{
				if (line.starts_with(">"))
				{
					if (!key.empty())
					{
						data.emplace(key, info.substr(0, info.length() - 1));
						info = "";
					}

					key = line.substr(1);
				}
				else if (!key.empty())
				{
					info.append(line).append("&");
				}
			}

			if (!key.empty())
			{
				data.emplace(key, info.substr(0, info.length() - 1));
			}

			return data;
		}

		std::array<int, DEFAULT_LEVEL_SIZE_X * DEFAULT_LEVEL_SIZE_Y> AssetManager::GetChunkData(const char* subfolder)
		{
			std::string filePath = AssetPath;
			filePath += subfolder;
			filePath += ChunkFileName;

			if (!std::filesystem::exists(filePath))
			{
				std::string message = "Missing chunk data file: \"" + filePath + "\"";
				CreateResoruceErrorMessage(message);
				return std::array<int, DEFAULT_LEVEL_SIZE_X* DEFAULT_LEVEL_SIZE_Y>();
			}

			std::ifstream ChunkFile(filePath);
			std::string text;

			std::array<int, DEFAULT_LEVEL_SIZE_Y * DEFAULT_LEVEL_SIZE_X> data;
			data.fill(0);

			size_t i = 0;

			while (std::getline(ChunkFile, text, ' '))
			{
				if (i >= DEFAULT_LEVEL_SIZE_X * DEFAULT_LEVEL_SIZE_Y) 
				{
#if _DEBUG
					DEBUG::DebugLog::LogWarning("AssetManager.cpp:162 - Chunk texture data file is larger than the given level size.");
#endif	
					return data;
				}
				data[i++] = std::stoi(text);
			}

			if (i < DEFAULT_LEVEL_SIZE_X * DEFAULT_LEVEL_SIZE_Y - 1)
			{
#if _DEBUG
				DEBUG::DebugLog::LogError("AssetManager.cpp:170 - Chunk texture data file is too small for the given level size!");
#endif	
			}

			return data;
		}

		

		void AssetManager::LoadObjectData(const char* subfolder, const std::function<void (std::vector<std::string>, size_t line)> mappings[], bool reloadObjects)
		{
			std::string filePath = AssetPath;
			filePath += subfolder;
			filePath += ObjectFileName;

			if (!std::filesystem::exists(filePath))
			{
				std::string message = "Missing object file: \"" + filePath + "\"";
				CreateResoruceErrorMessage(message);
				return;
			}

			std::ifstream ObjectFile(filePath);
			std::string line;

			std::vector<std::string> definedMacros;	//Stores a list of strings representing the templates that macros will use.
			definedMacros.reserve(5);
				
			std::vector<std::thread*> threads;
			threads.reserve(10);
			int thCurr = 0;

			bool declaringMacro = false;

			size_t lineID = 0;

			//Clear the object tags since we are loading a new level.
			Tags::ObjectTagManager::ClearTaggedObjects();

			while (std::getline(ObjectFile, line, '\n'))
			{
				if (line.empty() || line.starts_with(";")) { continue; } //For line breaks or lines beginning with a ";" do nothing. (useful for comments etc.)

#pragma region MacroDetection
				//Check if the line is declaring a macro.
				if (line.starts_with("-->"))
				{
					declaringMacro = !declaringMacro;
					
					if (declaringMacro)
					{
						definedMacros.push_back(std::regex_replace(line, std::regex("-->"), ""));
					}

					continue;
				}

				//Will continue defining a macro until another "-->" is found allowing for multi line macros.
				if (declaringMacro)
				{
					definedMacros[definedMacros.size() - 1] = definedMacros[definedMacros.size() - 1] + std::regex_replace(line, std::regex(" "), "");
					continue;
				}

				//Check if the line is using a macro.
				if (line.starts_with("<--"))
				{
					std::string resultingData;

					std::string s = std::regex_replace(line, std::regex("<--"), "");

					std::stringstream macrostream(s);
					std::string component;

					int c = 0;
					while (std::getline(macrostream, component, ','))
					{
						//First entry to the macro is which macro to use.
						if (c == 0)
						{
							int macro = std::stoi(component);
							resultingData = definedMacros[macro];
							c = 1;
							continue;
						}

						//Subsequent entries will be placed into the macro at %(c).
						resultingData = std::regex_replace(resultingData, std::regex("%" + std::to_string(c)), component == "null" ? "" : component);
						c++;
					}

					std::stringstream multiobjectmacrostream(resultingData);

					//If we have multiple objects in the macro to be defined they are separated by ;.
					while (std::getline(multiobjectmacrostream, component, ';'))
					{
						//Start the loading thread for the macro.
						threads.push_back(new std::thread(loadObjectDataThread, component, lineID++, mappings, thCurr, true));
						thCurr++;
					}

					goto checkthreads;
				}
#pragma endregion


				if (line.starts_with(LOAD_LAST_PREFIX))
				{
					m_loadAtEnd.push_back(line);
				}
				else {
					//Load the object. (if its not a load last.)
					threads.push_back(new std::thread(loadObjectDataThread, line, lineID++, mappings, thCurr, true));
					thCurr++;
				}

	checkthreads:
				if (thCurr == AppData::Settings::SettingsGlobals::MaxThreads.Value)
				{
					for (int i = 0; i < thCurr; i++)
					{
						if (threads[i] != nullptr && threads[i]->joinable())
						{
							threads[i]->join();
							//delete threads[i];
						}
					}

					thCurr = 0;
					m_currentThreadLoadCount.store(0);
					m_currentThreadLoadCountINT = 0;
				}
			}

			for (int i = 0; i < thCurr; i++)
			{
				if (threads[i] != nullptr && threads[i]->joinable())
				{
					threads[i]->join();
					//delete threads[i];
				}
			}

			//Load objects that must come last. (like water) (prefixed with '_m_')
			size_t lNum = 0;
			for (std::string line : m_loadAtEnd)
			{
				loadObjectDataThread(line.erase(0,3), lineID++, mappings);
			}

			m_loadAtEnd.clear();

			m_currentThreadLoadCount.store(0);

			//delete[definedMacrosCount] definedMacros;
		}

		/*/
			TODO: ADD ERROR HANDLING WITH SPECIFIC MESSAGE POPUPS TO THIS SYSTEM TO ENSURE IT IS OBVIOUS WHAT HAS OCCURED.
		*/
		void AssetManager::loadObjectDataThread(std::string line, size_t lineId, const std::function<void(std::vector<std::string>, size_t)> mappings[], int expectedLoadValue, bool expectLoad)
		{
			std::string objectTag = "";

			if (line.starts_with('|'))
			{
				objectTag = line.substr(1, line.find_last_of('|') - 1);
				line = line.substr(line.find_last_of('|') + 1);
			}

			std::stringstream linestream(line);
			std::string component;

			const std::function<void(std::vector<std::string>, size_t line)>* mapping{};

			std::vector <std::string> v;

			int c = 0;
			while (std::getline(linestream, component, ','))
			{
				if (c == 0)
				{
					int map = std::stoi(component);

					if (map > 19) 
					{ 
						mapping = nullptr;
					}
					else {
						mapping = &mappings[map];
					}

				}
				else {
					//Allows for the use of "5|4+" to mean 5+4=9 and plug 9 in for that variable in object.pk.
					int decodedComponent = 0;

					if (component.ends_with("+"))
					{
						component.erase(component.length() - 1);

						std::string decodedAddition = "";
						while (!component.ends_with("|"))
						{
							decodedAddition = component.at(component.length() - 1) + decodedAddition;
							component.erase(component.length() - 1);
						}

						component.erase(component.length() - 1);

						if (component.ends_with("t"))
						{
							component = std::to_string(std::stoi(component) * 8);
						}

						decodedComponent = std::stoi(decodedAddition) + std::stoi(component);
					}

					v.push_back(decodedComponent != 0 ? std::to_string(decodedComponent) : component);

				}
				c++; //C++ AHHAHAHAHAHAHH Like the language :)L::):)
			}

			//If we are expecting to load from the file, await for the array to be given the previous object in order to maintain load and render order.
			/*if (expectLoad)
			{
				std::cout << "THREAD " << expectedLoadValue << " WAITING\n";

				auto atmVal = m_currentThreadLoadCount.load(std::memory_order_relaxed);
				while (expectedLoadValue != atmVal)
				{
					//std::cout << "AWAITING " << expectedLoadValue << "!=" << atmVal << std::endl;
					atmVal = m_currentThreadLoadCount.load(std::memory_order_relaxed);
				}

				while (atmVal == expectedLoadValue)
				{
					if (!m_currentThreadLoadCount.compare_exchange_weak(atmVal, atmVal + 1, std::memory_order_release, std::memory_order_relaxed))
					{
						break;
					}
				}

				std::cout << "THREAD " << expectedLoadValue << " LOADED" << std::endl;
			}*/

			if (mapping == nullptr)
			{
				Mappings::LoadOver20Switch(std::stoi(line.substr(0, line.find_first_of(","))), v, lineId);

				if (!objectTag.empty())
				{
					Tags::ObjectTagManager::TagObject(Renderer::GetLastLoadedObject(), objectTag);
				}

				return;
			}

			(*mapping)(v, lineId);
			Tags::ObjectTagManager::TagObject(Renderer::GetLastLoadedObject(), objectTag);

			//delete mapping;	
		}


		
	}
}