#include "GreenRegion2LevelManager.h"

#include "../../TestGame.h"

#include "../../Items/FloorItem.h"
#include "../../Items/Types/Weapon.h"

#include "../../Objects/Enemies/Types/FeralRabbit.h"

#include "../../Objects/FunctionalObject.h"
#include "../../Objects/Environment/Effects/GlitchedRegion.h"

#include "../../../Utils/Time/GameTime.h"
#include "../../Objects/Environment/Effects/Explosion.h"

#include "../../Objects/Environment/Effects/GlitchEffect.h"

#include "../../../Objects/Tags/ObjectTagManager.h"

#include "../../Cutscenes/Character/CharacterAbility.h"

#include "../../Objects/Triggerable.h"

#define EVIL_WOLF_INTERACT_COORD_X 152.f * 8.f
#define EVIL_WOLF_SPAWN_COORD_X 192.f * 8.f

#define EVIL_WOLF_RUN_COORD_X 186.f * 8.f

#define FARMER_SPRITE SpriteBase(431)
#define COMPLETED_BUTTER_TASK_DIALOUGE "FinallyCreatedButter"

namespace GAME_NAME::Level
{
	using namespace Cutscenes;

	GameObject* GreenRegion2LevelManager::m_cutsceneWifiRouter = nullptr;
	double GreenRegion2LevelManager::m_cutsceneTimer = 0.0;

	AnimatingCharacter* GreenRegion2LevelManager::m_evilWolf = nullptr;
	AnimatingCharacter* GreenRegion2LevelManager::m_butterCraftChecker = nullptr;
	bool GreenRegion2LevelManager::m_playedEvilWolfCutscene = false, GreenRegion2LevelManager::m_wolfJumped = false;

	GreenRegion2LevelManager::GreenRegion2LevelManager()
	{
		//If we are in the barn, ignore the outdoor stuff.
		if (TestGame::INSTANCE->GetCurrentLevelPath().ends_with("barninterior"))
		{
			m_playedEvilWolfCutscene = true;

			FunctionalObject::RegisterFunction("WifiBreak", wifiBreakCutscene);

			//Create the guy at the end of the barn that asks for butter.
			m_butterCraftChecker = new AnimatingCharacter(Vec2{ 7784.f, 165.f }, Vec2{ 19.f, 26.308f }, true, 5.f, nullptr);
			m_butterCraftChecker->SetSprite(Renderer::GetSprite(FARMER_SPRITE));

			std::unordered_map<ITEM_TYPE, const ItemRecieveAndSpeakAbility::SpeechPattern> speechPatterns = {
				{ ITEM_TYPE::BUTTER, ItemRecieveAndSpeakAbility::SpeechPattern(">" + std::string(COMPLETED_BUTTER_TASK_DIALOUGE), Objects::Player::Player::NO_LOOK_DIRECTION, "")}
			};

			m_butterCraftChecker->AddAbility(new ItemRecieveAndSpeakAbility(speechPatterns));
			m_butterCraftChecker->AddAbility(new DesiredItemAbility(ITEM_TYPE::BUTTER, COMPLETED_BUTTER_TASK_DIALOUGE, ""));

			Renderer::InstantiateObject(Renderer::InstantiateGameObject(m_butterCraftChecker, true, 2, false));
		}
	}
	
	/// <summary>
	/// Used when checking if various events have occured during a cutscene.
	/// </summary>
	bool Cutscene_BooleanCheck = false;
	bool EndOfBarnButterRecieved = false;

#define WIFI_DOOR_TAG "WifiDoor"

	void GreenRegion2LevelManager::Update(GLFWwindow* window)
	{
		//Player is far enough over to consider this check plausible.
		if (TestGame::ThePlayer->GetPosition().X > 7000)
		{
			if (TestGame::INSTANCE->GetCurrentLevelPath().ends_with("barninterior"))
			{
				//Check if the player has delivered the butter at the end of the level.
				DesiredItemAbility* butterGiveCheck = dynamic_cast<DesiredItemAbility*>(m_butterCraftChecker->GetAbility(CharacterAbility::DesiredItem));
				if (butterGiveCheck->GetHasGiven())
				{
					GameObject* exitDoor = Objects::Tags::ObjectTagManager::GetObjectWithTag("ExitGarageDoorCover");
					if (exitDoor)
					{
						Triggerable* trigger = dynamic_cast<Triggerable*>(exitDoor);

						if (trigger)
						{
							trigger->Trigger(false, 1.f);
						}
					}

					EndOfBarnButterRecieved = true;
				}
			}
		}

		//Either it's barninterior or the cutscene has played.
		if (m_playedEvilWolfCutscene) {
			
			//The wifi router scene was triggered.
			if (m_cutsceneWifiRouter != nullptr)
			{

				//First trigger of the cutscene.
				if (!m_evilWolf)
				{
					const Vec2 routerPosition(5230.f, 32.f);

					m_evilWolf = new AnimatingCharacter(Vec2{ routerPosition.X + 100.f, routerPosition.Y }, Vec2{ DefaultPlayerScaleX, DefaultPlayerScaleY }, true, 5.f, (GAME_NAME::Objects::Player::Player::PlayerTextureData*)(&GAME_NAME::Objects::Player::Player::TextureData[GAME_NAME::Objects::Player::Player::TEXTURE_OFFSETS::DEFAULT_WOOF]));
					Renderer::InstantiateObject(Renderer::InstantiateGameObject(m_evilWolf, true, 1, false));

					m_evilWolf->SetTarget(Vec2{ routerPosition.X, routerPosition.Y });
				}

				if (m_evilWolf->AtTarget())
				{					
					if (m_cutsceneTimer > 2.f)
					{
						if (!m_evilWolf->IsFrozen())
						{
							m_evilWolf->SetFrozen(true, Objects::Player::Player::PLAYER_ANIMATION_STATE::BEHIND);
						}
					}

					if (m_cutsceneTimer > 5.f && m_cutsceneTimer < 6.f && !Cutscene_BooleanCheck /*Wifi hasn't been exploded*/)
					{
						Cutscene_BooleanCheck = true;	//Wifi has been exploded.
						Explosion* e = new Explosion(m_evilWolf->GetPosition() + m_evilWolf->GetScale() / 2.f, 10.f, 5.f);
						Renderer::InstantiateObject(Renderer::InstantiateGameObject(e, true, 1, false));
					}

					if (m_cutsceneTimer > 6.f && Cutscene_BooleanCheck /*Glitch region hasn't been created.*/)
					{
						Explosion* e = new Explosion(m_evilWolf->GetPosition() + m_evilWolf->GetScale() / 2.f, 30.f, 15.f);
						Renderer::InstantiateObject(Renderer::InstantiateGameObject(e, true, 1, false));

						GlitchedRegion* gr = new GlitchedRegion(Vec2{ m_evilWolf->GetPosition().X - 12.f, m_evilWolf->GetPosition().Y - 12.f }, Vec2{ 100.f, 100.f });
						gr->DynamicLoadGlitchedArea(); //The area was loaded after level loading, must call this.
						Renderer::InstantiateObject(Renderer::InstantiateGameObject(gr, false, 2, false));

						GameObject* wifiDoor = Tags::ObjectTagManager::GetObjectWithTag(WIFI_DOOR_TAG);
						if (wifiDoor)
						{
							GlitchEffect* objectGlitchEffect = new GlitchEffect(wifiDoor->GetPosition(), wifiDoor->GetScale());
							Renderer::InstantiateObject(Renderer::InstantiateGameObject(objectGlitchEffect, false, 2, false));
 						}

						Cutscene_BooleanCheck = false;
					}

					if (m_cutsceneTimer > 6.88f)
					{
						//Make the wolf run away.
						m_evilWolf->SetFrozen(false);
						m_evilWolf->SetTarget(Vec2{ m_evilWolf->GetPosition().X + 200.f, 32.f });
					}

					//Cutscene done, reset everything.
					if (m_cutsceneTimer > 7.88f)
					{
						//Remove the door to let the player through.
						GameObject* wifiDoor = Tags::ObjectTagManager::GetObjectWithTag(WIFI_DOOR_TAG);
						if (wifiDoor) { Renderer::DestroyObject(wifiDoor); }

						//Remove the wolf.
						Renderer::DestroyActiveObjectImmediate(m_evilWolf); m_evilWolf = nullptr;
						m_cutsceneWifiRouter = nullptr;
						m_cutsceneTimer = 0.0;
					}
				}

				m_cutsceneTimer += Utils::Time::GameTime::GetScaledDeltaTime();
			}

			return; 
		}

		//Check to play the wolf cutscene at the beginning.

		if (m_evilWolf == nullptr)
		{
			if (TestGame::ThePlayer->GetPosition().X > EVIL_WOLF_INTERACT_COORD_X)
			{
				m_evilWolf = new AnimatingCharacter(Vec2{ EVIL_WOLF_SPAWN_COORD_X, 21.f }, Vec2{ DefaultPlayerScaleX, DefaultPlayerScaleY }, true, 5.f, (GAME_NAME::Objects::Player::Player::PlayerTextureData*)(&GAME_NAME::Objects::Player::Player::TextureData[GAME_NAME::Objects::Player::Player::TEXTURE_OFFSETS::DEFAULT_WOOF]));
				Renderer::InstantiateObject(Renderer::InstantiateGameObject(m_evilWolf, true, 1, false));
			}
		}
		else if(TestGame::ThePlayer->GetPosition().X > EVIL_WOLF_RUN_COORD_X) {
			float xTarget = TestGame::INSTANCE->GetCamera()->GetPosition().X + TargetResolutionX;

			if (!m_wolfJumped)
			{
				m_wolfJumped = true;
				m_evilWolf->Jump();

				Items::FloorItem* screwdriver = new Items::FloorItem(m_evilWolf->GetPosition(), new Items::Weapon(ITEM_TYPE::SCREWDRIVER, 0), 0.5f);
				Renderer::InstantiateObject(Renderer::InstantiateGameObject(screwdriver, true, 1, false));

				Enemies::FeralRabbit* rabbit = new Enemies::FeralRabbit(Vec2{ xTarget + 30.f, 21.f }, Vec2{ 8.f, 6.f }, Renderer::GetSprite(92), 93);
				Renderer::InstantiateObject(Renderer::InstantiateGameObject(rabbit, true, 2, false));
			}

			if (m_evilWolf->IsOnGround())
			{
				m_evilWolf->SetTarget(Vec2{ xTarget + (m_evilWolf->GetScale().X * 2), m_evilWolf->GetPosition().Y });
			}

		
			if (m_evilWolf->GetPosition().X > xTarget || m_evilWolf->GetPosition().Y < 0.f)
			{
				Renderer::DestroyActiveObject(m_evilWolf);
				m_playedEvilWolfCutscene = true;
				m_evilWolf = nullptr;
			}
		}

	}

	void GreenRegion2LevelManager::wifiBreakCutscene(GameObject* wifiRouter)
	{
		m_cutsceneWifiRouter = wifiRouter;
	}
}
