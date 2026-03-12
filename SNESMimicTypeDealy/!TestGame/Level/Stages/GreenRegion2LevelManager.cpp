#include "GreenRegion2LevelManager.h"

#include "../../TestGame.h"

#include "../../Items/FloorItem.h"
#include "../../Items/Types/Weapon.h"

#include "../../Objects/Enemies/Types/FeralRabbit.h"

#define EVIL_WOLF_INTERACT_COORD_X 152.f * 8.f
#define EVIL_WOLF_SPAWN_COORD_X 192.f * 8.f

#define EVIL_WOLF_RUN_COORD_X 186.f * 8.f

namespace GAME_NAME::Level
{
	using namespace Cutscenes;

	AnimatingCharacter* GreenRegion2LevelManager::m_evilWolf = nullptr;
	bool GreenRegion2LevelManager::m_playedEvilWolfCutscene = false, GreenRegion2LevelManager::m_wolfJumped = false;

	GreenRegion2LevelManager::GreenRegion2LevelManager()
	{
		//If we are in the barn, ignore the outdoor stuff.
		if (TestGame::INSTANCE->GetCurrentLevelPath().ends_with("barninterior"))
		{
			m_playedEvilWolfCutscene = true;
		}
	}

	void GreenRegion2LevelManager::Update(GLFWwindow* window)
	{
		if (m_playedEvilWolfCutscene) { return; }

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
}
