#include "LevelCompleteZone.h"

#include "../TestGame.h"
#include "../../Utils/CollisionDetection.h"
#include "../../Utils/Time/GameTime.h"

#define LEVEL_COMPLETE_ZONE_BALLOON_SPRITE SpriteBase(258)
#define LEVEL_COMPLETE_ZONE_BALLOON_SCALE Vec2{ 11.f, 29.f }
#define LEVEL_PLAYER_HOLD_BALLOON_OFFSET Vec2{ 3.f, -10.f }

#define LEVEL_COMPLETE_ZONE_CONFETTI_SPRITE SpriteBase(261)
#define LEVEL_COMPLETE_ZONE_CONFETTI_OFFSET Vec2{ 6.f, 3.f }

namespace GAME_NAME::Objects
{
	LevelCompleteZone::LevelCompleteZone(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, std::string destination)
		: GameObject(position, scale, sprite, 0.0f), m_levelDestination(destination)
	{

	}

	void LevelCompleteZone::Update(GLFWwindow* window)
	{
		if (m_currentScene != nullptr)
		{
			m_currentScene->Update();

			if (m_currentScene->IsDone())
			{
				//Ensure this object doesn't become unloaded so it keeps updating.
				if (m_position.Y < TestGame::ThePlayer->GetPosition().Y - TargetResolutionY)
				{
					m_position.Y = TestGame::ThePlayer->GetPosition().Y - TargetResolutionY;
				}

				m_currentScene.release();

				TestGame::SetLoadLevelWithSavedPlayer(false);
				TestGame::INSTANCE->LoadLevelAndAllData(m_levelDestination.c_str());

				m_currentScene = nullptr;
			}

			return;
		}

		if (CollisionDetection::BoxWithinBox(TestGame::ThePlayer->GetPosition(), TestGame::ThePlayer->GetScale(), m_position, m_scale))
		{			
			m_currentScene.reset(new ConfettiLevelCompleteScene());
		}
	}

	LevelCompleteZone::BalloonLevelCompleteScene::BalloonLevelCompleteScene()
		: m_balloonObject(new GameObject(Vec2{ TestGame::ThePlayer->GetPosition().X, -LEVEL_COMPLETE_ZONE_BALLOON_SCALE.Y }, LEVEL_COMPLETE_ZONE_BALLOON_SCALE, Renderer::GetSprite(LEVEL_COMPLETE_ZONE_BALLOON_SPRITE)))
	{
		TestGame::ThePlayer->SetFrozen(true, Player::Player::NO_LOOK_DIRECTION);
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(m_balloonObject, false, 2, true));
	}

	LevelCompleteZone::BalloonLevelCompleteScene::~BalloonLevelCompleteScene()
	{
		if (m_balloonObject != nullptr)
		{
			Renderer::DestroyActiveObject(m_balloonObject);
		}
	}

	LevelCompleteZone::ConfettiLevelCompleteScene::ConfettiLevelCompleteScene()
		: m_elapsedTime(0.0)
	{
		TestGame::ThePlayer->SetFrozen(false);
		TestGame::ThePlayer->SetFrozen(true, Player::Player::VICTORY_CONFETTI);

		m_confettiEmitter = new Particles::ParticleEmitter(TestGame::ThePlayer->GetPosition() + LEVEL_COMPLETE_ZONE_CONFETTI_OFFSET, 1.f, false);
		Particles::Particle confetti(Vec2::Zero, Vec2{ 8 }, 0.f, Vec2{ 0.f, 0.f }, 3.f, 1.f, Renderer::GetSprite(LEVEL_COMPLETE_ZONE_CONFETTI_SPRITE));
		confetti.TargetOpacity = 0;
		m_confettiEmitter->RegisterParticle(confetti);
		Renderer::LoadActiveObject(m_confettiEmitter);
		m_confettiEmitter->SpawnParticlesLooping(0.05, 3, Vec2{ 5.f, -0.5f }, 5.f, 3.f);
	}

	LevelCompleteZone::ConfettiLevelCompleteScene::~ConfettiLevelCompleteScene()
	{
		Renderer::DestroyActiveObject(m_confettiEmitter);
	}

	void LevelCompleteZone::BalloonLevelCompleteScene::Update()
	{
		//Move balloon up.
		if (m_balloonObject != nullptr)
		{
			m_balloonObject->Translate(Vec2{ 0.f, (float)Utils::Time::GameTime::GetScaledDeltaTime() * 32.f });

			if (TestGame::ThePlayer->GetLookDirection() == Player::Player::VICTORY_BALLOON)
			{
				TestGame::ThePlayer->SetPosition(m_balloonObject->GetPosition() + LEVEL_PLAYER_HOLD_BALLOON_OFFSET);
				TestGame::ThePlayer->SetGravity(0.f);
				TestGame::ThePlayer->SetAirTime(0.f);
			}
			//The balloon has reached the player.
			else if (std::abs(TestGame::ThePlayer->GetPosition().Y - m_balloonObject->GetPosition().Y) < 8.f)
			{
				TestGame::ThePlayer->SetFrozen(false);
				TestGame::ThePlayer->SetFrozen(true, Player::Player::VICTORY_BALLOON);
			}
			else {
				m_balloonObject->SetPosition(Vec2{ TestGame::ThePlayer->GetPosition().X, m_balloonObject->GetPosition().Y });
			}
		}
	}

	void LevelCompleteZone::ConfettiLevelCompleteScene::Update()
	{
		m_elapsedTime += Utils::Time::GameTime::GetScaledDeltaTime();
		TestGame::ThePlayer->Translate(Vec2{ 0.f, (float)m_elapsedTime * 3.f });

		m_confettiEmitter->SetPosition(TestGame::ThePlayer->GetPosition() + LEVEL_COMPLETE_ZONE_CONFETTI_OFFSET);
	}

	bool LevelCompleteZone::BalloonLevelCompleteScene::IsDone()
	{
		//Player has floated up enough.
		return TestGame::ThePlayer->GetPosition().Y > TargetResolutionY + TestGame::ThePlayer->GetScale().Y;
	}

	bool LevelCompleteZone::ConfettiLevelCompleteScene::IsDone()
	{
		//Player has floated up enough.
		return TestGame::ThePlayer->GetPosition().Y > TargetResolutionY + TestGame::ThePlayer->GetScale().Y;
	}
}

