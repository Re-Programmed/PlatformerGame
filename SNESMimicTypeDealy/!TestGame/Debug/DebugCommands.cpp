#ifdef _DEBUG

#include "DebugCommands.h"
#include <thread>
#include "../TestGame.h"
#include <iostream>

#include "../../Debug/DebugLog.h"

#include "../Objects/Environment/Effects/Rain.h"
#include "../Objects/Environment/Effects/Explosion.h"

#include "../../Rendering/Lighting/SimpleLightingManager.h"

#include "../../Input/InputManager.h"

#include "../../Utils/Time/GameTime.h"

#include "../../Objects/StateSaver.h"
#include "../Objects/StartState.h"
#include "../Mappings.h"

#include "../Camera/LevelBuilderCam.h"

#include "../TestGame.h"

#include "../Level/LevelManager.h"

#include "../Items/Crafting/CraftingMenuManager.h"

#include "../Objects/Environment/Effects/ElectricalZap.h"

#include "../../Objects/GUI/Menus/GUIMenu.h"
#include "../Objects/GUI/GUIItemSelectionBox.h"

#include "../Level/Hub/HouseManager.h"

#include "../Objects/Player/Currency.h"

#include "../../Audio/SoundManager.h"

#include "../Cutscenes/DialogueManager.h"

#define DebugCommands_Log(x) DEBUG::DebugLog::Log(std::string("[Debug Commands] ").append(x), true, ";33");

std::vector<std::string> DebugCommands::m_queuedCommands = std::vector<std::string>(2);

#define DecodePos(x,val) x == "p" ? GAME_NAME::TestGame::ThePlayer->GetPosition().val : std::stof(x)

void DebugCommands::HandleKeybinds()
{
	if (GAME_NAME::InputManager::GetKeyUpDown(GAME_NAME::keyRef::DEBUG_OBJECT_MENU) & GAME_NAME::InputManager::KEY_STATE::KEY_STATE_PRESSED)
	{
		m_queuedCommands.push_back("loadlevel,/green_region");
	}
}

void DebugCommands::RunRecieverThread()
{

	std::thread debugRecieveThread([]() {

		//Random sleep.
		Sleep(1000);
		DEBUG::DebugLog::Log("[Debug Commands] Initilized!", true, ";33");

		while (!glfwWindowShouldClose(GAME_NAME::TestGame::FirstWindow))
		{
			std::string input;
			std::cin >> input;

			m_queuedCommands.push_back(input);
		}
		});

	debugRecieveThread.detach();
}

void DebugCommands::HandleCommands()
{
	HandleKeybinds();
	

	for (auto input : m_queuedCommands)
	{
		if (input == "ping")
		{
			DEBUG::DebugLog::Log("[Debug Commands] Pong", true, ";33");
			continue;
		}

		if (input == "dcam")
		{
			DebugCommands_Log("Enabled Debug Camera.");

			GAME_NAME::Camera::LevelBuilderCam* lbc = new GAME_NAME::Camera::LevelBuilderCam(GAME_NAME::TestGame::INSTANCE->GetCamera()->GetPosition(), GAME_NAME::TestGame::INSTANCE->GetCamera()->GetZoom());

			GAME_NAME::TestGame::INSTANCE->SetCamera(lbc);

			continue;
		}

		if (input.starts_with("playsound"))
		{
			std::vector<std::string> params = getParams(input);

			if (params.size() < 1)
			{
				DebugCommands_Log("Provide a sound. (sound_key,audio_group=SFX,volume=-1.f,panning=0.f)");
				continue;
			}

			GAME_NAME::Audio::AudioGroup group = GAME_NAME::Audio::SoundManager::SFXGroup;
			GAME_NAME::Audio::AudioBus bus = GAME_NAME::Audio::SoundManager::SFXBus;
			float volume = -1.f, panning = 0.f;

			if (params.size() > 1)
			{
				if (params[1] == "Music")
				{
					group = GAME_NAME::Audio::SoundManager::MusicGroup;
					bus = GAME_NAME::Audio::SoundManager::MusicBus;
				}
			}

			if (params.size() > 2)
			{
				volume = std::stof(params[2]);
			}


			if (params.size() > 3)
			{
				panning = std::stof(params[3]);
			}

			GAME_NAME::Audio::SoundManager::Play(params[0], group, volume, panning, false, bus.mChannelHandle);
			DebugCommands_Log("Played sound: " + params[0] + ".");
			continue;
		}

		if (input == "rainon")
		{
			DebugCommands_Log("Toggled Rain On.");

			GAME_NAME::Objects::Environment::Effects::Rain::StartRainEffect();
			continue;
		}

		if (input == "rainoff")
		{
			DebugCommands_Log("Toggled Rain Off.");

			GAME_NAME::Objects::Environment::Effects::Rain::EndRainEffect();
			continue;
		}

		if (input == "dive")
		{
			DebugCommands_Log("Diving.");
			GAME_NAME::TestGame::ThePlayer->Dive(Vec2{ 100.f, 80.f }, 15.f);
			continue;
		}

		if (input.starts_with("explosion"))
		{
			std::vector<std::string> params = getParams(input);

			if(params.size() < 4)
			{
				DebugCommands_Log("Provide a location, radius, and power for the explosion. (x,y,r,pow)");
				continue;
			}

			Vec2 pos(DecodePos(params[0], X), DecodePos(params[1], Y));
			
			GAME_NAME::Objects::Environment::Effects::Explosion* expl = new GAME_NAME::Objects::Environment::Effects::Explosion(pos, std::stof(params[2]), std::stof(params[3]));
			Renderer::LoadActiveObject(expl);

			std::string message("Creating Explosion at ");
			message.append(pos.ToString());
		    DebugCommands_Log(message);
			continue;
		}

		if (input == "lightingon")
		{
			GAME_NAME::Rendering::Lighting::SimpleLightingManager::EnableLighting(DEFAULT_LEVEL_SIZE_X/10, true);
			DebugCommands_Log("Lighting Enabled!");

			continue;
		}

		if (input == "lightingoff")
		{
			GAME_NAME::Rendering::Lighting::SimpleLightingManager::DisableLighting();
			DebugCommands_Log("Lighting Disabled!");

			continue;
		}

		if (input == "worklight")
		{
			GAME_NAME::Rendering::Lighting::SimpleLightingManager::RegisterSource(new Lighting::LightingSource(GAME_NAME::InputManager::GetMouseWorldPosition(GAME_NAME::TestGame::INSTANCE->GetCamera()), 100.f, 25.f, Lighting::POINT_LIGHT));
			DebugCommands_Log("Created work light.");

			continue;
		}

		if (input.starts_with("timescale"))
		{
			std::vector<std::string> params = getParams(input);

			if (params.size() < 1) { DebugCommands_Log("Provide a new time scale (float)."); continue; }

			GAME_NAME::Utils::Time::GameTime::SetTimeScale(std::stof(params[0]));
			DebugCommands_Log("Set timescale.");

			continue;
		}

		if (input.starts_with("savestates"))
		{
			GAME_NAME::Objects::StateSaver::SaveStates();
			GAME_NAME::Objects::StateSaver::SaveMisc();

			DebugCommands_Log("Saved all states.");

			continue;
		}

		if (input.starts_with("startstate"))
		{
			GAME_NAME::Objects::StateSaver::SaveStates();
			GAME_NAME::Objects::StateSaver::SaveMisc();
			GAME_NAME::Objects::StartState::CreateStartState();


			DebugCommands_Log("Saved all states and created a start state.");

			continue;
		}


		if (input.starts_with("createzap"))
		{
			std::vector<std::string> params = getParams(input);

			if (params.size() < 4) { DebugCommands_Log("Provide a origin and destination (x1,y1,x2,y2)."); continue; }

			GAME_NAME::Objects::Environment::Effects::ElectricalZap::CreateZapBetweenPoints(Vec2{ std::stof(params[0]), std::stof(params[1]) }, Vec2{ std::stof(params[2]), std::stof(params[3]) });

			DebugCommands_Log("Created a zap.");

			continue;
		}

		if (input.starts_with("givescore"))
		{
			std::vector<std::string> params = getParams(input);

			if (params.size() < 1) { DebugCommands_Log("Provide a score (score)."); continue; }

			GAME_NAME::Level::GlobalLevelData::UpdatePoints(std::stoi(params[0]), GAME_NAME::TestGame::ThePlayer->GetPosition());

			DebugCommands_Log("Gave " + params[0] + " points.");

			continue;
		}

		if (input.starts_with("loadlevel"))
		{
			std::vector<std::string> params = getParams(input);

			if (params.size() < 1) { DebugCommands_Log("Provide a level path."); continue; }

			GAME_NAME::Resources::SaveManager::SetCurrentFile("default_s");

			GAME_NAME::Renderer::ClearObjects();
			GAME_NAME::Renderer::ClearTextures();

			GAME_NAME::TestGame::SetLoadLevelWithSavedPlayer(true);

			GAME_NAME::TestGame::INSTANCE->LoadLevel(params[0].c_str(), GAME_NAME::TestGame::LEVEL_DATA_TEXTURES_BACKGROUND, true);
			GAME_NAME::TestGame::INSTANCE->LoadLevel("/global_assets", GAME_NAME::TestGame::LEVEL_DATA_TEXTURES_SPRITES);
			GAME_NAME::TestGame::INSTANCE->LoadLevel(params[0].c_str(), static_cast<GAME_NAME::Game::Game::LEVEL_DATA>(GAME_NAME::TestGame::LEVEL_DATA_ALL xor GAME_NAME::TestGame::LEVEL_DATA_TEXTURES_BACKGROUND xor GAME_NAME::TestGame::LEVEL_DATA_DATA_LEVEL));
			GAME_NAME::Mappings::LoadObjectsWithDefaultMapping(params[0].c_str());
			GAME_NAME::TestGame::INSTANCE->LoadLevel(params[0].c_str(), static_cast<GAME_NAME::Game::Game::LEVEL_DATA>(GAME_NAME::TestGame::LEVEL_DATA_DATA_LEVEL));
			GAME_NAME::Cutscenes::DialogueManager::INSTANCE->LoadStoredDialogueSequences(params[0].c_str());
			GAME_NAME::TestGame::INSTANCE->RenderFront = true;
			DebugCommands_Log("Loaded a level.");

			continue;
		}

		if (input == "wipe")
		{
			GAME_NAME::Level::LevelManager::LevelCircleAnimation(GAME_NAME::TestGame::ThePlayer->GetPosition() - GAME_NAME::TestGame::INSTANCE->GetCamera()->GetPosition());

			DebugCommands_Log("Played circle wipe.");

			continue;
		}

		if (input == "getpos")
		{			
			DebugCommands_Log("The player is at: " + GAME_NAME::TestGame::ThePlayer->GetPosition().ToString());

			continue;
		}
		
		if (input == "craftingmenu")
		{
			GAME_NAME::Items::Crafting::CraftingMenuManager::OpenCraftingMenu();

			DebugCommands_Log("Opened crafting menu.");
			
			continue;
		}

		if (input == "furnituremenu")
		{
			GAME_NAME::Level::HouseManager::OpenFurnitureInventory();

			DebugCommands_Log("Opened furniture menu.");

			continue;
		}

		if (input == "storefurniture")
		{
			GAME_NAME::Level::HouseManager::StorePlayerInventory();

			DebugCommands_Log("Stopred furniture.");

			continue;
		}

		if (input.starts_with("loadmenu"))
		{
			std::vector<std::string> params = getParams(input);

			if (params.size() < 1) { DebugCommands_Log("Provide a menu path."); continue; }

			GAME_NAME::Objects::GUI::Menus::GUIMenu::LoadMenu(params[0].c_str(), nullptr);

			DebugCommands_Log("Loaded " + params[0] + ".");

			continue;
		}

		if (input.starts_with("givecrumbs"))
		{
			std::vector<std::string> params = getParams(input);
			if (params.size() < 1) { DebugCommands_Log("How many?"); continue; }

			int amount = std::stoi(params[0]);

			GAME_NAME::Objects::Player::Currency::AddCrumbs(amount);
			DebugCommands_Log("Gave " + std::to_string(amount) + " crumbs.");
		}

		if (input.starts_with("giveitemcode"))
		{
			std::vector<std::string> params = getParams(input);
			if (params.size() < 1) { DebugCommands_Log("Provide an item code."); continue; }

			std::string itemParam;
			for (std::string str : params)
			{
				itemParam.append(",");
				itemParam.append(str);
			}

			itemParam = itemParam.substr(1);

			InventoryItem* item = InventoryItem::DecodeItemString(itemParam);
			DebugCommands_Log("Gave " + ITEMTYPE_GetItemTypeName(item->GetType()) + ".");

			int slot = GAME_NAME::TestGame::ThePlayer->GetInventory()->AddItem(item);
			if (slot < 0)
			{
				slot = GAME_NAME::TestGame::ThePlayer->GetBackpack()->AddItem(item);

				if (slot < 0)
				{
					DebugCommands_Log("Inventory full!");
				}
			}
		}else if (input.starts_with("giveitem"))
		{
			std::vector<std::string> params = getParams(input);

			if (params.size() < 1) { DebugCommands_Log("Provide an item name."); continue; }

			std::string itemParam;
			for (std::string str : params)
			{
				itemParam.append(" ");
				itemParam.append(str);
			}

			itemParam = itemParam.substr(1);

			int i = 0;
			for (ItemData data : GAME_NAME::Items::ITEM_DATA)
			{
				if (data.DisplayName == itemParam)
				{
					DebugCommands_Log("Gave " + itemParam + ".");
					int slot = GAME_NAME::TestGame::ThePlayer->GetInventory()->AddItem(new InventoryItem(static_cast<ITEM_TYPE>(i)));
					if (slot < 0)
					{
						slot = GAME_NAME::TestGame::ThePlayer->GetBackpack()->AddItem(new InventoryItem(static_cast<ITEM_TYPE>(i)));

						if (slot < 0)
						{
							DebugCommands_Log("Inventory full!");
						}
					}

					goto end_command_check;
				}

				i++;
			}

			DebugCommands_Log("Could not find " + itemParam + ".");

			int iter = 0;
			for (ItemData data : GAME_NAME::Items::ITEM_DATA)
			{
				DebugCommands_Log(std::to_string(iter++) + ": " + data.DisplayName);
			}

			continue;
		}
	

	}

end_command_check:
	m_queuedCommands.clear();
}

std::vector<std::string> DebugCommands::getParams(std::string input)
{
	std::vector<std::string> output;

	std::stringstream ss(input);
	std::string line;

	while (std::getline(ss, line, ','))
	{
		output.push_back(line);
	}

	output.erase(output.begin());
	return output;
}

#endif