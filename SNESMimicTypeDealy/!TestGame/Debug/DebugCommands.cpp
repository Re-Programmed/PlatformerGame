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
		
		if (input == "crafting_menu")
		{
			GAME_NAME::Items::Crafting::CraftingMenuManager::OpenCraftingMenu();

			DebugCommands_Log("Opened crafting menu.");
			
			continue;
		}
	}

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