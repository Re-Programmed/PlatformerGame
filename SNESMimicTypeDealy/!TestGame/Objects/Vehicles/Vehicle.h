#pragma once

#include "../../../Objects/Helpers/Interactable.h"

#include "../../../Components/Physics/Collision/ActiveBoxCollider.h"

namespace GAME_NAME::Objects::Vehicles
{
	class Vehicle
		: public Interactable
	{
	public:
		void Render(const Vec2& cameraPos);
		void Update(GLFWwindow* window);

		void onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state) override;

		virtual void Mount(std::shared_ptr<Player::Player>& player);
		virtual void Dismount(std::shared_ptr<Player::Player>& player);

		///Called each frame the vehicle is in use.
		virtual void Use();

	protected:
		bool m_inUse = false;

		struct VehicleData
		{
			const Player::Player::PLAYER_ANIMATION_STATE UseAnimation;
			const Vec2 Scale;
			const int Sprite;	//Using the sprite's get value, not its index.
			const Vec2 Speed = { 500.f , 0.f };
		};

		Vehicle(Vec2 position, const VehicleData data);

		static void onCollisionS(Vec2 push, Objects::GameObject* obj);

	private:
		const VehicleData m_storedData;

		static void onCollision(Vec2 push, GameObject* object);
	};
}