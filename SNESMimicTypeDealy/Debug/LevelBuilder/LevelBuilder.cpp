#if _DEBUG

#include "LevelBuilder.h"
#include "../../Resources/AssetManager.h"
#include "../../Input/InputManager.h"
#include "../../!TestGame/TestGame.h"
#include "../../!TestGame/Mappings.h"
#include "../../Rendering/DynamicSprite.h"
#include "../../Objects/GUI/GUIManager.h"
		

//The area around the mouse that can be used to select objects.
constexpr unsigned int MouseSelectionRadius = 16U;

namespace GAME_NAME::Debug::LevelBuilder
{
	LevelBuilder* LevelBuilder::sv_currentLevelBuilder;
	bool LevelBuilder::m_objectMenuOpen = false;

	GUI::GUIButton* LevelBuilder::m_addObjectButtons[LVLBUILDER_SPAWNABLE_COUNT];

	const Vec4 RedTextureColor[4] = {
		Vec4(1, 0, 0, 1),
		Vec4(1, 0, 0, 1),
		Vec4(1, 0, 0, 1),
		Vec4(1, 0, 0, 1)
	};

	void LevelBuilder::InitLevelBuilderAssets(GAME_NAME::Game::Game* game)
	{
		//Load level editor textures.
		game->LoadLevel("/debug_level_builder_assets", Game::Game::LEVEL_DATA_TEXTURES_SPRITES);
	}

	void LevelBuilder::InitLevelBuilder(GAME_NAME::Game::Game* game)
	{
		//game->LoadLevel("/town_1", Game::Game::LEVEL_DATA_TEXTURES_BACKGROUND, true);
		//game->LoadLevel("/global_assets", Game::Game::LEVEL_DATA_TEXTURES_SPRITES);
		//game->LoadLevel("/town_1", (GAME_NAME::Game::Game::LEVEL_DATA)(Game::Game::LEVEL_DATA_ALL xor Game::Game::LEVEL_DATA_TEXTURES_BACKGROUND));

		const int spriteCount = Renderer::GetSpriteCount() + Renderer::GetBackgroundSpriteCount();

		//Create a new loaded level builder.
		m_objectMenuOpen = false;

		if (sv_currentLevelBuilder != nullptr)
		{
			delete sv_currentLevelBuilder;
		}

		new LevelBuilder(game, spriteCount);
	}

	void LevelBuilder::Render()
	{
		if (sv_currentLevelBuilder == nullptr) { return; }

		DrawObjectMenu();

		DynamicSprite dsobj = DynamicSprite(sv_currentLevelBuilder->m_loadedSpriteCount);
		dsobj.Render(sv_currentLevelBuilder->m_currentGame->GetCamera()->GetPosition(), InputManager::GetMouseWorldPosition(sv_currentLevelBuilder->m_currentGame->GetCamera()) - Vec2(MouseSelectionRadius / 2), Vec2(MouseSelectionRadius));

		if (sv_currentLevelBuilder->m_editObject != nullptr)
		{
			//Render selected object.
			DynamicSprite ds = DynamicSprite(sv_currentLevelBuilder->m_editObject->GetSprite()->GetSpriteId());
			ds.UpdateTextureColor(RedTextureColor);
			ds.Render(sv_currentLevelBuilder->m_currentGame->GetCamera()->GetPosition(), sv_currentLevelBuilder->m_editObject->GetPosition() + sv_currentLevelBuilder->m_editObject->GetScale(), sv_currentLevelBuilder->m_editObject->GetScale(), sv_currentLevelBuilder->m_editObject->GetRotation() + 180.f);
		}

	}

	void LevelBuilder::DrawObjectMenu()
	{
		auto ds = Renderer::GetDynamicSprite(SpriteBase(-1));
		Rendering::Camera::Camera* c = sv_currentLevelBuilder->m_currentGame->GetCamera();
		ds->Render(c->GetPosition(), c->UIToGlobal(Vec2(0.f, 0.f)), Vec2(TargetResolutionX/5.5f, TargetResolutionY));

using namespace GUI;

		if (m_objectMenuOpen)
		{
			return;
		}

		m_objectMenuOpen = true;
		for (int i = 0; i < LVLBUILDER_SPAWNABLE_COUNT; i++)
		{
			SpawnObjects::Spawnable* sObj = SpawnObjects::GetSpawnable(i);

			GUIButton* btn = new GUIButton(Vec2(0.f, (float)i * 30.f), Vec2(30.f, 30.f), sObj->Spawnables[0].MyObject->GetSprite()->GetSpriteId(), new std::function([](int spawnable) {
				SpawnObjects::Spawnable* spawnableObj = SpawnObjects::GetSpawnable(spawnable);

				for (GAME_NAME::Rendering::Renderer::InstantiateGameObject obj : spawnableObj->Spawnables)
				{
					obj.MyObject->SetPosition(TestGame::ThePlayer->GetPosition() + obj.MyObject->GetPosition());
					Renderer::InstantiateObject(obj);

					std::cout << "Spawning object: " << spawnable << ", at: " << obj.MyObject->GetPosition().ToString() << std::endl;
				}

				delete spawnableObj;
				}), i);

			delete sObj;

			m_addObjectButtons[i] = btn;

			Renderer::LoadGUIElement(btn, LevelBuilderGuiLayer);
			GUIManager::RegisterButton(btn);
		}
	}

	bool PerformedButtonClickEvent = false;

	enum EditableParams
	{
		X_POSITION,
		Y_POSITION,
		NONE
	};

	EditableParams currentReadParam;

	int64_t readingNumInput = 0;
	int8_t currReadDigit = 0;
	bool readDigitDown = false;

	//Reads the values getting inputted by the keyboard. (returns the result in readingNumInput)
	void readNumInput()
	{
		bool gotKey = false;

		for (int i = 0; i < 10; i++)
		{
			if (InputManager::GetKey(GLFW_KEY_0 + i))
			{
				gotKey = true;
				if (!readDigitDown)
				{
					readingNumInput *= 10;
					readingNumInput += i;
					currReadDigit += 1;

					DEBUG::DebugLog::Log("Read: " + std::to_string(readingNumInput));

					readDigitDown = true;
				}
			}
		}

		if (InputManager::GetKey(GLFW_KEY_ENTER))
		{
			currReadDigit = -1;

			DEBUG::DebugLog::Log("Read: >ENTER");
		}

		if (InputManager::GetKey(GLFW_KEY_ESCAPE))
		{
			currReadDigit = -2;

			DEBUG::DebugLog::Log("Read: >ESC");
		}

		if (!gotKey)
		{
			readDigitDown = false;
		}
	}

	bool refreshingLevel = false;

	void LevelBuilder::Update(GLFWwindow* window)
	{
		if (InputManager::GetKeyUpDown(PLAYER_DEBUG_TOGGLE_FLIGHT) & InputManager::KEY_STATE_PRESSED)
		{
			TestGame::ThePlayer->ToggleFlight();
		}

		if (InputManager::GetKey(DEBUG_REFRESH_LEVEL_FILES))
		{
			if (!refreshingLevel)
			{
				Vec2 storedPlayerPos = TestGame::ThePlayer->GetPosition();

				refreshingLevel = true;
				Renderer::UpdateObjects = false;

				
				TestGame::INSTANCE->ClearLevel(TestGame::LEVEL_DATA_ALL);
				Renderer::ClearObjects();

				TestGame::INSTANCE->LoadLevel("/town_1", TestGame::LEVEL_DATA_TEXTURES_BACKGROUND);

				TestGame::INSTANCE->LoadLevel("/global_assets", TestGame::LEVEL_DATA_TEXTURES_SPRITES);

				TestGame::INSTANCE->LoadLevel("/town_1", (GAME_NAME::Game::Game::LEVEL_DATA)(TestGame::LEVEL_DATA_TEXTURES_BACKGROUND xor TestGame::LEVEL_DATA_ALL));

				GAME_NAME::Mappings::LoadObjectsWithDefaultMapping("/town_1");
				Renderer::UpdateObjects = true;

				TestGame::ThePlayer->SetPosition(storedPlayerPos);
				TestGame::ThePlayer->EnterDebug();
				
			}				
		}
		else {
			refreshingLevel = false;
		}

		if (InputManager::GetMouseButton(0) || InputManager::GetMouseButton(1) || InputManager::GetControllerButton(0, controllerRef::BUTTON_ZL) || InputManager::GetControllerButton(0, controllerRef::BUTTON_L) || InputManager::GetControllerButton(0, controllerRef::BUTTON_ZR) || InputManager::GetControllerButton(0, controllerRef::BUTTON_R))
		{
			if (PerformedButtonClickEvent == false)
			{
				PerformedButtonClickEvent = true;
				GUI::GUIManager::ButtonClickEvent(InputManager::GetMouseScreenPosition(), InputManager::GetMouseButton(1));
			}
		}
		else {
			PerformedButtonClickEvent = false;
		}

		if (m_editObject != nullptr && currentReadParam == NONE)
		{
			if (InputManager::GetKey(DEBUG_SET_OBJECT_X))
			{
				DEBUG::DebugLog::Log(">>> Setting X (Current: " + std::to_string(m_editObject->GetPosition().X) + ")");
				currentReadParam = X_POSITION;
				readingNumInput = 0;
			}

			if (InputManager::GetKey(DEBUG_SET_OBJECT_Y))
			{
				DEBUG::DebugLog::Log(">>> Setting Y (Current: " + std::to_string(m_editObject->GetPosition().Y) + ")");
				currentReadParam = Y_POSITION;
				readingNumInput = 0;
			}
		}

		if (currentReadParam != NONE)  
		{
			readNumInput();
		}

		//A value has been entered.
		if(currReadDigit == -1) {
			switch (currentReadParam)
			{
			case X_POSITION:
				if (m_editObject != nullptr)
				{
					m_editObject->SetPosition(Vec2((float)readingNumInput, m_editObject->GetPosition().Y));
				}
				break;
			case Y_POSITION:
				if (m_editObject != nullptr)
				{
					m_editObject->SetPosition(Vec2(m_editObject->GetPosition().X, (float)readingNumInput));
				}
				break;
			}

			currReadDigit = 0;
			readingNumInput = 0;
			currentReadParam = NONE;
		}
		else if (currReadDigit == -2)
		{
			currReadDigit = 0;
			readingNumInput = 0;
			currentReadParam = NONE;
		}

		if (InputManager::GetMouseButton(1))
		{
			if (m_interactKeyDown)
			{
				return;
			}

			m_interactKeyDown = true;

			Vec2 cPos = InputManager::GetMouseWorldPosition(TestGame::INSTANCE->GetCamera());

			GameObject* closestObj = nullptr;
			float shortestDist = 100.f;

			std::vector<GameObject*> objs = Renderer::GetAllObjectsInArea(cPos - Vec2(MouseSelectionRadius/2), Vec2(MouseSelectionRadius), true);

			std::cout << objs.size() << " : SIZE" << std::endl;

			int c = 0;
			for (GameObject* obj : objs)
			{
				const float d = Vec2::Distance(obj->GetPosition(), cPos);
				if (d < shortestDist && m_editObject != obj)
				{
					closestObj = obj;
					shortestDist = d;
				}
				c++;
			}

			std::cout << "Found " << c << " objects." << std::endl;

			if (closestObj != nullptr)
			{
				m_editObject = closestObj;
			}
		}
		else {
			m_interactKeyDown = false;
		}
	}


	void LevelBuilder::DestroyLevelBuilder()
	{

		if (sv_currentLevelBuilder != nullptr)
		{
			UpdateManager::RemoveUpdateable(sv_currentLevelBuilder->m_updateIndex);

			for (int i = 0; i < LVLBUILDER_SPAWNABLE_COUNT; i++)
			{
				Renderer::UnloadGUIElement(sv_currentLevelBuilder->m_addObjectButtons[i], LevelBuilderGuiLayer);
				GUI::GUIManager::UnregisterButton(sv_currentLevelBuilder->m_addObjectButtons[i]);

				delete sv_currentLevelBuilder->m_addObjectButtons[i];
			}

			delete sv_currentLevelBuilder;
		}
	}
}

#endif
