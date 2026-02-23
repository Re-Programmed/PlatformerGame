#pragma once

#include "../../../Objects/GameObject.h"
#include "../../../Components/Physics/Collision/Helpers/ActiveBoxCollisionGravityObject.h"
#include "../../../Rendering/Sprite.h"

#include "../../Objects/Player/Player.h"

using namespace GAME_NAME::Objects;
using namespace GAME_NAME::Rendering;
using namespace GAME_NAME::Components::Physics::Collision;

namespace GAME_NAME::Cutscenes
{
	constexpr float DefaultCharacterGravity = 5.5f;
	constexpr float DefaultCharacterStopDistance = 5.f;
	constexpr float DefaultCharacterSpeed = 0.04f;
	constexpr float DefaultCharacterSpeedCap = 160.f;

	constexpr float DefaultCharacterJumpHeight = 94.f;

	constexpr float DefaultCharacterFriction = 16.f;

	class AnimatingCharacter
		: public ActiveBoxCollisionGravityObject
	{
	public:
		AnimatingCharacter(Vec2 position, Vec2 scale, bool gravityObserved, float stopDistance = DefaultCharacterStopDistance, Objects::Player::Player::PlayerTextureData* textureData = new Objects::Player::Player::PlayerTextureData() /*Use default bird sprites.*/);

		void Update(GLFWwindow* window) override;

		void Render(const Vec2& cameraPos) override;

		void SetTarget(Vec2 targetLocation);
		void SetTarget(GameObject* object);

		void Jump();

		void beforeCollision() override;
		void onCollision(Vec2 push, GameObject* self, GameObject* other) override;

		typedef struct TargetPoint
		{
			Vec2 Location;
			GameObject* Object;
		};

		
		~AnimatingCharacter()
		{
			delete m_textureData;
		}

	protected:
		/// <summary>
		/// All reference textures to use.
		/// </summary>
		Objects::Player::Player::PlayerTextureData* m_textureData;

		TargetPoint m_target;
		/// <summary>
		/// How much space the character should aim to place between themselves and the target.
		/// </summary>
		float m_stopDistance = DefaultCharacterStopDistance;

		/// <summary>
		/// How fast the character can accelerate.
		/// </summary>
		float m_speed = DefaultCharacterSpeed;
		/// <summary>
		/// The maximum speed the character can attain.
		/// </summary>
		float m_speedCap = DefaultCharacterSpeedCap;

		Components::Animation::AnimatorComponent* m_animator = nullptr;

		void pathfind();
		void updateAnimations();

		void registerAnimations();

		inline bool isOnGround() { return m_onGround; }

	private:
		bool m_onGround = false;
		bool m_gravityObserved;

		float m_timeSpentNotMoving = 0.f;
	};
}