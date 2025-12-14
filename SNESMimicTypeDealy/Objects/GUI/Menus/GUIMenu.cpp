#include "GUIMenu.h"

#include "../../../Settings/SettingsGlobals.h"
#include "../../../Resources/AssetManager.h"
#include <fstream>
#include <mutex>
#include <thread>

#if _DEBUG
#include "../../../Debug/DebugLog.h"
#endif

#include "../GUIManager.h"
#include "../Progress/ProgressBar.h"

#include "../Text/TextRenderer.h"

#define LoadMenuW LoadMenu

namespace GAME_NAME::Objects::GUI::Menus
{
	std::mutex GUIMenu::m_asyncAddLock;
	std::mutex asyncInstantiationLock;

	uint8_t GUIMenu::m_menusOpen = 0;

	uint8_t GUIMenu::ScrollAreaElementCounter = 0;
	GUIScrollArea* GUIMenu::LastScrollArea = nullptr;

	uint16_t GUIMenu::m_lastMenuObjects[3] = {
		0,0,0
	};

	std::vector<unsigned int> GUIMenu::m_lastRegisteredButtonIds;

	void GUIMenu_checkScrollAreaAppend(StaticGUIElement* const element)
	{
		if (GUIMenu::ScrollAreaElementCounter > 0)
		{
			GUIMenu::LastScrollArea->AddElement(element);

			GUIMenu::ScrollAreaElementCounter--;
			if (GUIMenu::ScrollAreaElementCounter <= 0)
			{
				GUIMenu::LastScrollArea = nullptr;
			}
		}
	}

	void GUIMenu_checkScrollAreaAppend(const Text::TextRenderer::RenderedWord& word)
	{
		if (GUIMenu::ScrollAreaElementCounter > 0)
		{
			for (Text::TextRenderer::Letter letter : word)
			{
				GUIMenu::LastScrollArea->AddElement(letter);
			}

			GUIMenu::ScrollAreaElementCounter--;
			if (GUIMenu::ScrollAreaElementCounter <= 0)
			{
				GUIMenu::LastScrollArea = nullptr;
			}
		}
	}

	void GUIMenu_loadMenuObjectThread(std::string line, std::function<void(int)>* elementCallback)
	{
#if _DEBUG
		DEBUG::DebugLog::Log("[Menu] Loading object with string \"" + line + "\".");
#endif

		std::string typeCode = std::string(line).erase(3, line.length());
		GUIManager::GUI_ELEMENT_TYPE gtype = GUIManager::GetGUIComponentFromCode(typeCode);

		std::stringstream ComponentStream(std::string(line).erase(0, 4));
		std::string component;
		std::vector<std::string> data;

		while (std::getline(ComponentStream, component, ','))
		{
			if (component.ends_with("sb"))
			{
				component.erase(component.size() - 2, component.size());
				component = std::to_string(SpriteBase(std::stoi(component)));
			}

			data.push_back(component);
		}

		switch (gtype)
		{
			//Creates an EL_TEXT (text,pos1,pos2,scale[float],padding[float],layer)
			case GUIManager::EL_TEXT:
			{
using namespace Text;

				asyncInstantiationLock.lock();
				TextRenderer::RenderedWord wObj = TextRenderer::RenderWord(data[0], { std::stof(data[1]), std::stof(data[2]) }, std::stof(data[3]), std::stof(data[4]), std::stoi(data[5]));
				asyncInstantiationLock.unlock();

				GUIMenu::AddLastMenuObject(std::stoi(data[5]), static_cast<int>(wObj.size()));
				GUIMenu_checkScrollAreaAppend(wObj);

				break;
			}
			//Creates an EL_BUTTON (pos1, pos2, sca1, sca2, sprite, buttonID, baseColorR, baseColorG, baseColorB, baseColorA, hoverColorR, hoverColorG, hoverColorB, hoverColorA) 
			case GUIManager::EL_BUTTON:
			{
				Vec4 baseColor = { 1.f, 1.f, 1.f, 1.f };
				Vec4 hoverColor = { 0.5f, 0.5f, 0.f, 1.f };
				if (data.size() > 6)
				{
					baseColor = { std::stof(data[6]), std::stof(data[7]), std::stof(data[8]), std::stof(data[9]) };

					if (data.size() > 10)
					{
						hoverColor = { std::stof(data[10]), std::stof(data[11]), std::stof(data[12]), std::stof(data[13]) };
					}
				}

				std::cout << "LOADED BUTTON WITH ID: " << std::stoi(data[5]) << std::endl;
				GUIButton* gButton = new GUIButton(Vec2(std::stof(data[0]), std::stof(data[1])), Vec2(std::stof(data[2]), std::stof(data[3])), Renderer::GetSprite(std::stoi(data[4]))->GetSpriteId(), elementCallback, std::stoi(data[5]), baseColor, hoverColor);
				
				asyncInstantiationLock.lock();
				Renderer::LoadGUIElement(gButton);
				asyncInstantiationLock.unlock();

				GUIMenu::AddLastRegisterButtonID(GUIManager::RegisterButton(gButton));
				GUIMenu::AddLastMenuObject(1);

				GUIMenu_checkScrollAreaAppend(gButton);

				break;
			}
			///Creates an EL_BLANK (pos1, pos2, sca1, sca2, sprite, layer)
			case GUIManager::EL_BLANK:
			{
				StaticGUIElement* gBlank = new StaticGUIElement(Vec2(std::stof(data[0]), std::stof(data[1])), Vec2(std::stof(data[2]), std::stof(data[3])), Renderer::GetSprite(std::stoi(data[4]))->GetSpriteId());
				Renderer::LoadGUIElement(gBlank, std::stoi(data[5]));

				asyncInstantiationLock.lock();
				GUIMenu::AddLastMenuObject(std::stoi(data[5]));
				asyncInstantiationLock.unlock();

				GUIMenu_checkScrollAreaAppend(gBlank);

				break;
			}
			case GUIManager::EL_PROGRESS:
			{
				ProgressBar* pBar = new ProgressBar(Vec2(std::stof(data[0]), std::stof(data[1])), Vec2(std::stof(data[2]), std::stof(data[3])), Renderer::GetSprite(std::stoi(data[4]))->GetSpriteId(), std::stoi(data[5]));

				asyncInstantiationLock.lock();
				Renderer::LoadGUIElement(pBar);
				asyncInstantiationLock.unlock();

				GUIMenu::AddLastMenuObject(1);

				GUIMenu_checkScrollAreaAppend(pBar);

				break;
			}

			//Creates an EL_SCROLLAREA (posx, posy, scalex, scaley, bgTexture (can be "null"), scrollYMin, scrollYMax, itemCount)
			case GUIManager::EL_SCROLLAREA:
			{
				Sprite* sp = data[4] == "null" ? nullptr : Renderer::GetSprite(std::stoi(data[4]));
				GUIScrollArea* scrollArea = new GUIScrollArea(Vec2(std::stof(data[0]), std::stof(data[1])), Vec2(std::stof(data[2]), std::stof(data[3])), data[4] == "null" ? 0 : sp->GetSpriteId(), std::stod(data[5]), std::stod(data[6]));
				delete sp;

				asyncInstantiationLock.lock();
				Renderer::LoadGUIElement(scrollArea);
				asyncInstantiationLock.unlock();

				GUIMenu::AddLastMenuObject(1);

				GUIMenu::ScrollAreaElementCounter = std::stoi(data[7]);
				GUIMenu::LastScrollArea = scrollArea;

				break;
			}
		}
	}

	unsigned int GUIMenu::LoadMenu(const char* menu, std::function<void(int)>* elementCallback)
	{
		GUIManager::MenuIsOpen = true;
		m_menusOpen++;

		m_lastMenuObjects[0] = 0; m_lastMenuObjects[1] = 0; m_lastMenuObjects[2] = 0;
		m_lastRegisteredButtonIds.clear();

		std::string filePath = AssetPath;
		filePath += MenuSubfolder;
		filePath += menu;
		filePath += MenuFileExtension;

		std::ifstream MenuStream(filePath);
		std::string line;

		std::vector<std::thread*> threads;
		uint16_t thCurr = 0;

		while (std::getline(MenuStream, line, '\n'))
		{
			if (line.empty() || line.starts_with(";")) { continue; } //For line breaks or lines beginning with a ";" do nothing. (useful for comments etc.)

			//Buttons should be loaded in main thread to prevent bugs with buttonID getting off sync.
			//if (line.starts_with("btn")) { GUIMenu_loadMenuObjectThread(line, elementCallback); continue; }

			//Removed thread system.
			GUIMenu_loadMenuObjectThread(line, elementCallback);

			//threads.push_back(new std::thread(GUIMenu_loadMenuObjectThread, line, elementCallback));
			//thCurr++;

		//checkthreads:
			/*
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
			}
			*/
		}

		for (int i = 0; i < thCurr; i++)
		{
			if (threads[i] != nullptr && threads[i]->joinable())
			{
				threads[i]->join();
				//delete threads[i];
			}
		}

		return m_lastRegisteredButtonIds.size() == 0 ? 0 : m_lastRegisteredButtonIds[0];
	}

	void GUIMenu::RemoveLastMenu()
	{
		m_menusOpen--;

		if (m_menusOpen == 0)
		{
			GUIManager::MenuIsOpen = false;
		}

		for (int i = 0; i < 3; i++)
		{
			if (m_lastMenuObjects[i] > 0)
			{
				Renderer::ClearGUIObjects(m_lastMenuObjects[i], i);
			}
		}

		for (unsigned int i : m_lastRegisteredButtonIds)
		{
			GUIManager::UnregisterButton(0u);
		}
	}

	void GUIMenu::OpenMenu()
	{
		m_menusOpen++;
		GUIManager::MenuIsOpen = true;
	}

}

