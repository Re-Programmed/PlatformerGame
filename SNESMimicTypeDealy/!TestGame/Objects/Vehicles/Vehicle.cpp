#include "Vehicle.h"

#include "../../InputDisplay/DisplayIconManager.h"

#include "../../../Utils/Time/GameTime.h"

namespace GAME_NAME::Objects::Vehicles
{
	Vehicle::Vehicle(Vec2 position, const VehicleData data)
		: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, data.Scale.X/2.f, position, data.Scale, Renderer::GetSprite(data.Sprite), true), m_storedData(data)
	{

	}


	void Vehicle::Render(const Vec2& cameraPos)
	{
		//When in use, the player sprite will have the vehicle in it.
		if (m_inUse)
		{
			return;
		}

		Interactable::Render(cameraPos);
	}


	void Vehicle::Update(GLFWwindow* window)
	{
		if (m_inUse)
		{
			this->SetPosition(TestGame::ThePlayer->GetPosition());

			Use();
		}

		Interactable::Update(window);

	}

	void Vehicle::onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state)
	{
		if (!m_inUse)
		{
			Input::DisplayIconManager::ShowKeyInputDisplay(keyRef::PLAYER_INTERACT, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), 0);
		}

		if (InputManager::GetKeyUpDown(keyRef::PLAYER_INTERACT) & InputManager::KEY_STATE_PRESSED)
		{
			if (m_inUse)
			{
				Dismount(player);
			}
			else {
				Mount(player);
			}
		}
	}

	void Vehicle::Use()
	{
		if (InputManager::GetKey(keyRef::PLAYER_MOVE_LEFT))
		{
			float dt = static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime());
			TestGame::ThePlayer->AddVelocity({ -m_storedData.Speed.X * dt, -m_storedData.Speed.Y * dt });
		}

		if (InputManager::GetKey(keyRef::PLAYER_MOVE_RIGHT))
		{
			float dt = static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime());
			TestGame::ThePlayer->AddVelocity({ m_storedData.Speed.X * dt, m_storedData.Speed.Y * dt });
		}
	}

	void Vehicle::Mount(std::shared_ptr<Player::Player>& player)
	{
		m_inUse = true;
		player->SetPosition(Vec2{ player->GetPosition().X, this->m_position.Y });

		player->SetFrozen(true, Player::Player::BIKING);
	}

	void Vehicle::Dismount(std::shared_ptr<Player::Player>& player)
	{
		m_inUse = false;
		player->SetFrozen(false);
	}


}
