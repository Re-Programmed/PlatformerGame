#include "DepartManager.h"

#include "../../../Objects/GUI/GUIManager.h"
#include "../../../Resources/Save/SaveManager.h"
#include "../../Mappings.h"
#include "../../Cutscenes/DialogueManager.h"
#include "../../Objects/StartState.h"

#define DEPART_MANAGER_BACKING_TEXTURE 18
#define DEPART_MANAGER_LEVEL_TAB_BUTTON_TEXTURE 19
#define DEPART_MANAGER_LEVEL_AREA_BUTTON_TEXTURE 22
#define DEPART_MANAGER_LEVEL_AREA_BELL_BUTTON_TEXTURE 24

constexpr float DEPART_MANAGER_TRANSITION_POINT_X = 1480;

constexpr const char* DEPART_MANAGER_SELECTED_LEVEL_SAVE = "selected_level";
constexpr const char* DEPART_MANAGER_SELECTED_AREA_SAVE = "selected_area";

namespace GAME_NAME::Level
{
	using namespace GUI;

	DepartManager::DepartureSelectMenu DepartManager::m_menu;
	
	DepartManager::LevelID DepartManager::m_currentDisplayedLevel;
	uint8_t DepartManager::m_currentSelectedArea = 1;

	DepartManager::LevelSelector DepartManager::m_levelSelections( 
		std::array<LevelSelectionData, DEPART_MANAGER_LEVEL_COUNT> {
			DepartManager::LevelSelectionData{ LevelID::GreenRegion, 21, "/green_region", "/green_region", "/green_region", "Area One", "Area Two", "Area Three", true }
		}
	);

	void DepartManager::Update()
	{
		if (TestGame::ThePlayer->GetPosition().X > DEPART_MANAGER_TRANSITION_POINT_X)
		{
			//Load the selected level as the player has passed the fog.
			// 
			//TODO: Figure out save file stuff.
			GAME_NAME::Resources::SaveManager::SetCurrentFile("default_s");

			GAME_NAME::Renderer::ClearObjects();
			GAME_NAME::Renderer::ClearTextures();

			GAME_NAME::TestGame::SetLoadLevelWithSavedPlayer(false);

			std::string levelPath = (m_currentSelectedArea == 1 ? m_levelSelections.Get().at(static_cast<int>(m_currentDisplayedLevel)).Name1
				: (m_currentSelectedArea == 2 ? m_levelSelections.Get().at(static_cast<int>(m_currentDisplayedLevel)).Name2 
				: m_levelSelections.Get().at(static_cast<int>(m_currentDisplayedLevel)).Name3));


			//Load selected level.
			GAME_NAME::TestGame::INSTANCE->LoadLevel(levelPath.c_str(), GAME_NAME::TestGame::LEVEL_DATA_TEXTURES_BACKGROUND, true);
			GAME_NAME::TestGame::INSTANCE->LoadLevel("/global_assets", GAME_NAME::TestGame::LEVEL_DATA_TEXTURES_SPRITES);
			GAME_NAME::TestGame::INSTANCE->LoadLevel(levelPath.c_str(), static_cast<GAME_NAME::Game::Game::LEVEL_DATA>(GAME_NAME::TestGame::LEVEL_DATA_ALL xor GAME_NAME::TestGame::LEVEL_DATA_TEXTURES_BACKGROUND xor GAME_NAME::TestGame::LEVEL_DATA_DATA_LEVEL));
			GAME_NAME::Mappings::LoadObjectsWithDefaultMapping(levelPath.c_str());
			GAME_NAME::TestGame::INSTANCE->LoadLevel(levelPath.c_str(), static_cast<GAME_NAME::Game::Game::LEVEL_DATA>(GAME_NAME::TestGame::LEVEL_DATA_DATA_LEVEL));
			Cutscenes::DialogueManager::INSTANCE->LoadStoredDialogueSequences(levelPath.c_str());
			GAME_NAME::TestGame::INSTANCE->RenderFront = true;

			//Save the player at the enterance to the level. (If they die now they will go back to the level beginning.)
			GAME_NAME::Objects::StateSaver::SaveStates();
			GAME_NAME::Objects::StateSaver::SaveMisc();
			GAME_NAME::Objects::StartState::CreateStartState();
		}
	}

	void DepartManager::Load()
	{
		m_levelSelections.AllowSave();
		m_levelSelections.Load();

		m_menu.Interactable = new DepartMenuInteractable();
		Renderer::LoadObject(m_menu.Interactable);
	}

	//Accounts for the scaling up of the backing texture to match the screen height.
	constexpr float spriteScalingFactor = TargetResolutionY / 64.f;
	constexpr float positionOffsetFactor = TargetResolutionX / 2.f - TargetResolutionY / 2.f;

	void DepartManager::OpenDepartMenu()
	{
		if (m_menu.Backing != nullptr) { return; }

		//Load current selected level data.
		int displayedLevel = 0, selectedArea = 1;
		Resources::SaveManager::GetSaveInteger(DEPART_MANAGER_SELECTED_LEVEL_SAVE, displayedLevel);
		Resources::SaveManager::GetSaveInteger(DEPART_MANAGER_SELECTED_AREA_SAVE, selectedArea);
		m_currentDisplayedLevel = static_cast<LevelID>(displayedLevel);
		m_currentSelectedArea = static_cast<uint8_t>(selectedArea);

		Sprite* sp = Renderer::GetSprite(SpriteBase(-1));
		m_menu.DarkenedBack = new StaticGUIElement(Vec2{ 0 }, Vec2{ TargetResolutionX, TargetResolutionY }, sp->GetSpriteId());
		Renderer::LoadGUIElement(m_menu.DarkenedBack, 1);
		delete sp;

		sp = Renderer::GetSprite(DEPART_MANAGER_BACKING_TEXTURE);
		m_menu.Backing = new StaticGUIElement(Vec2{ positionOffsetFactor, 0.f }, Vec2{ TargetResolutionY }, sp->GetSpriteId());
		delete sp;
		Renderer::LoadGUIElement(m_menu.Backing);

		sp = Renderer::GetSprite(DEPART_MANAGER_LEVEL_TAB_BUTTON_TEXTURE + 1);
		m_menu.LevelAdvanceButton = new GUIButton(Vec2{ positionOffsetFactor + 2.f * spriteScalingFactor, 25.f * spriteScalingFactor }, Vec2{ 5.f * spriteScalingFactor, 29.f * spriteScalingFactor }, sp->GetSpriteId(), new std::function<void(int)>(DepartManager::buttonCallback), Vec4{ 1.f, 1.f, 1.f, 1.f }, Vec4{ 0.5f, 0.5f, 0.5f, 1.f });
		Renderer::LoadGUIElement(m_menu.LevelAdvanceButton);
		GUIManager::RegisterButton(m_menu.LevelAdvanceButton);
		delete sp;

		sp = Renderer::GetSprite(DEPART_MANAGER_LEVEL_TAB_BUTTON_TEXTURE);
		m_menu.LevelBackButton = new GUIButton(Vec2{ positionOffsetFactor + 57.f * spriteScalingFactor, 25.f * spriteScalingFactor }, Vec2{ 5.f * spriteScalingFactor, 29.f * spriteScalingFactor }, sp->GetSpriteId(), new std::function<void(int)>(DepartManager::buttonCallback), Vec4{ 1.f, 1.f, 1.f, 1.f }, Vec4{ 0.5f, 0.5f, 0.5f, 1.f });
		Renderer::LoadGUIElement(m_menu.LevelBackButton);
		GUIManager::RegisterButton(m_menu.LevelBackButton);
		delete sp;

		sp = Renderer::GetSprite(DEPART_MANAGER_LEVEL_AREA_BUTTON_TEXTURE + (m_currentSelectedArea == 1));
		m_menu.Area1Button = new GUIButton(Vec2{ positionOffsetFactor + 23.f * spriteScalingFactor, 19.f * spriteScalingFactor }, Vec2{ 31.f * spriteScalingFactor, 5.f * spriteScalingFactor }, sp->GetSpriteId(), new std::function<void(int)>(DepartManager::buttonCallback), Vec4{ 1.f, 1.f, 1.f, 1.f }, Vec4{ 0.5f, 0.7f, 0.5f, 1.f });
		Renderer::LoadGUIElement(m_menu.Area1Button);
		GUIManager::RegisterButton(m_menu.Area1Button);
		delete sp;

		sp = Renderer::GetSprite(DEPART_MANAGER_LEVEL_AREA_BUTTON_TEXTURE + (m_currentSelectedArea == 2));
		m_menu.Area2Button = new GUIButton(Vec2{ positionOffsetFactor + 23.f * spriteScalingFactor, 13.f * spriteScalingFactor }, Vec2{ 31.f * spriteScalingFactor, 5.f * spriteScalingFactor }, sp->GetSpriteId(), new std::function<void(int)>(DepartManager::buttonCallback), Vec4{ 1.f, 1.f, 1.f, 1.f }, Vec4{ 0.5f, 0.7f, 0.5f, 1.f });
		Renderer::LoadGUIElement(m_menu.Area2Button);
		GUIManager::RegisterButton(m_menu.Area2Button);
		delete sp;

		sp = Renderer::GetSprite(DEPART_MANAGER_LEVEL_AREA_BUTTON_TEXTURE + (m_currentSelectedArea == 3));
		m_menu.Area3Button = new GUIButton(Vec2{ positionOffsetFactor + 23.f * spriteScalingFactor, 7.f * spriteScalingFactor }, Vec2{ 31.f * spriteScalingFactor, 5.f * spriteScalingFactor }, sp->GetSpriteId(), new std::function<void(int)>(DepartManager::buttonCallback), Vec4{ 1.f, 1.f, 1.f, 1.f }, Vec4{ 0.5f, 0.7f, 0.5f, 1.f });
		Renderer::LoadGUIElement(m_menu.Area3Button);
		GUIManager::RegisterButton(m_menu.Area3Button);
		delete sp;

		sp = Renderer::GetSprite(DEPART_MANAGER_LEVEL_AREA_BELL_BUTTON_TEXTURE);
		m_menu.Bell = new GUIButton(Vec2{ positionOffsetFactor + 6.f * spriteScalingFactor, 7.f * spriteScalingFactor }, Vec2{ 8.f * spriteScalingFactor, 6.f * spriteScalingFactor }, sp->GetSpriteId(), new std::function<void(int)>(DepartManager::buttonCallback), Vec4{ 1.f, 1.f, 1.f, 0.0f }, Vec4{ 1.0f, 1.0f, 1.0f, 0.66f });
		Renderer::LoadGUIElement(m_menu.Bell, 2);
		GUIManager::RegisterButton(m_menu.Bell);
		delete sp;

		//TODO: Make the next avaiable level.
		LoadLevelDisplay(LevelID::GreenRegion);
	}

	bool DepartManager::CloseDepartMenu()
	{
		if (m_menu.Backing == nullptr)
		{
			return false;
		}
		
		//Save current selected level data.
		Resources::SaveManager::SaveInteger(static_cast<int>(m_currentDisplayedLevel), DEPART_MANAGER_SELECTED_LEVEL_SAVE);
		Resources::SaveManager::SaveInteger(m_currentSelectedArea, DEPART_MANAGER_SELECTED_AREA_SAVE);

		Renderer::UnloadGUIElement(m_menu.Backing);
		delete m_menu.Backing; m_menu.Backing = nullptr;
		Renderer::UnloadGUIElement(m_menu.DarkenedBack);
		delete m_menu.DarkenedBack;
		GUIManager::UnregisterButton(m_menu.Area1Button);
		Renderer::UnloadGUIElement(m_menu.Area1Button);
		delete m_menu.Area1Button;
		GUIManager::UnregisterButton(m_menu.Area2Button);
		Renderer::UnloadGUIElement(m_menu.Area2Button);
		delete m_menu.Area2Button;
		GUIManager::UnregisterButton(m_menu.Area3Button);
		Renderer::UnloadGUIElement(m_menu.Area3Button);
		delete m_menu.Area3Button;

		GUIManager::UnregisterButton(m_menu.LevelAdvanceButton);
		Renderer::UnloadGUIElement(m_menu.LevelAdvanceButton);
		delete m_menu.LevelAdvanceButton;
		GUIManager::UnregisterButton(m_menu.LevelBackButton);
		Renderer::UnloadGUIElement(m_menu.LevelBackButton);
		delete m_menu.LevelBackButton;

		Renderer::UnloadGUIElement(m_menu.LevelDisplay);
		delete m_menu.LevelDisplay;

		Text::TextRenderer::UnloadWord(m_menu.Area1Title, 2);
		Text::TextRenderer::UnloadWord(m_menu.Area2Title, 2);
		Text::TextRenderer::UnloadWord(m_menu.Area3Title, 2);

		GUIManager::UnregisterButton(m_menu.Bell);
		Renderer::UnloadGUIElement(m_menu.Bell, 2);

		return true;
	}

	void DepartManager::LoadLevelDisplay(LevelID level)
	{
		for (LevelSelectionData data : m_levelSelections.Get())
		{
			if (data.LevelID == level)
			{
				//Don't display locked levels.
				if (!data.IsUnlocked()) { return; }

				m_currentDisplayedLevel = level;

				Sprite* sp = Renderer::GetSprite(data.LevelDisplaySprite);
				m_menu.LevelDisplay = new StaticGUIElement(Vec2{ positionOffsetFactor + 11.f * spriteScalingFactor, 26.f * spriteScalingFactor }, Vec2{ 42.f * spriteScalingFactor, 28.f * spriteScalingFactor }, sp->GetSpriteId());
				Renderer::LoadGUIElement(m_menu.LevelDisplay);
				delete sp;

				m_menu.Area1Title = Text::TextRenderer::RenderWordCaseSensitive(data.Area1, Vec2{ positionOffsetFactor + 27.f * spriteScalingFactor, 20.f * spriteScalingFactor }, 8.f, -2.f, 2, -191, -109, std::chrono::milliseconds(150));
				m_menu.Area2Title = Text::TextRenderer::RenderWordCaseSensitive(data.Area2, Vec2{ positionOffsetFactor + 27.f * spriteScalingFactor, 14.f * spriteScalingFactor }, 8.f, -2.f, 2, -191, -109, std::chrono::milliseconds(150));
				m_menu.Area3Title = Text::TextRenderer::RenderWordCaseSensitive(data.Area3, Vec2{ positionOffsetFactor + 27.f * spriteScalingFactor, 8.f * spriteScalingFactor }, 8.f, -2.f, 2, -191, -109, std::chrono::milliseconds(150));

				return;
			}
		}
	}

	void DepartManager::buttonCallback(int buttonID)
	{
		if (m_menu.Backing == nullptr) { return; }

		if (buttonID == m_menu.LevelBackButton->GetButtonId())
		{
			if (static_cast<int>(m_currentDisplayedLevel) - 1 < 0)
			{
				m_currentDisplayedLevel = static_cast<LevelID>(DEPART_MANAGER_LEVEL_COUNT - 1);
				return;
			}

			m_currentDisplayedLevel = static_cast<LevelID>(static_cast<int>(m_currentDisplayedLevel) - 1);
		}
		else if (buttonID == m_menu.LevelAdvanceButton->GetButtonId())
		{
			if (static_cast<int>(m_currentDisplayedLevel) + 1 >= DEPART_MANAGER_LEVEL_COUNT)
			{
				m_currentDisplayedLevel = static_cast<LevelID>(0);
				return;
			}

			m_currentDisplayedLevel = static_cast<LevelID>(static_cast<int>(m_currentDisplayedLevel) + 1);
		}
		else if (buttonID == m_menu.Area1Button->GetButtonId())
		{
			m_menu.Area1Button->SetSprite(Renderer::GetSprite(DEPART_MANAGER_LEVEL_AREA_BUTTON_TEXTURE + 1));
			m_menu.Area2Button->SetSprite(Renderer::GetSprite(DEPART_MANAGER_LEVEL_AREA_BUTTON_TEXTURE));
			m_menu.Area3Button->SetSprite(Renderer::GetSprite(DEPART_MANAGER_LEVEL_AREA_BUTTON_TEXTURE));

			m_currentSelectedArea = 1;
		}
		else if (buttonID == m_menu.Area2Button->GetButtonId())
		{
			m_menu.Area1Button->SetSprite(Renderer::GetSprite(DEPART_MANAGER_LEVEL_AREA_BUTTON_TEXTURE));
			m_menu.Area2Button->SetSprite(Renderer::GetSprite(DEPART_MANAGER_LEVEL_AREA_BUTTON_TEXTURE + 1));
			m_menu.Area3Button->SetSprite(Renderer::GetSprite(DEPART_MANAGER_LEVEL_AREA_BUTTON_TEXTURE));

			m_currentSelectedArea = 2;
		}
		else if (buttonID == m_menu.Area3Button->GetButtonId())
		{
			m_menu.Area1Button->SetSprite(Renderer::GetSprite(DEPART_MANAGER_LEVEL_AREA_BUTTON_TEXTURE));
			m_menu.Area2Button->SetSprite(Renderer::GetSprite(DEPART_MANAGER_LEVEL_AREA_BUTTON_TEXTURE));
			m_menu.Area3Button->SetSprite(Renderer::GetSprite(DEPART_MANAGER_LEVEL_AREA_BUTTON_TEXTURE + 1));

			m_currentSelectedArea = 3;
		}
		else if (buttonID == m_menu.Bell->GetButtonId())
		{
			//Play bell sound.
		}
	}

	void DepartManager::LevelSelectionData::Unlock()
	{
		m_unlocked = true;
	}

	void DepartManager::LevelSelectionData::Decode(const SaveParam params)
	{
		if (params == "t")
		{
			Unlock();
		}
	}

	MiscState::SaveParam DepartManager::LevelSelectionData::Encode()
	{
		return SaveParam(m_unlocked ? "t" : "f");
	}
}