#pragma once
#include "../../../Components/Physics/Collision/Helpers/StaticBoxCollisionObject.h"

#include "../../../Components/Animation/AnimatorComponent.h"

#include <memory>

using namespace GAME_NAME::Components::Physics::Collision;
using namespace GAME_NAME::MathUtils;


namespace GAME_NAME::Objects::Platforms
{
	class JointSpinPlatform
		: public StaticBoxCollisionObject
	{
	public:
		JointSpinPlatform(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, double fallTime, bool FB = false);

	protected:
		/// <summary>
		/// onCollision checks if an object lands on the platform, causing it to fall.
		/// </summary>
		/// <param name="push"></param>
		/// <param name="object"></param>
		/// <param name="collider"></param>
		/// <returns></returns>
		static bool onCollision(Vec2 push, Objects::GameObject* object, Objects::GameObject* collider);

		void Update(GLFWwindow* window) override;
	private:
		const bool m_isForwardBackward;
		bool m_hasFallen = false;

		std::unique_ptr<Components::Animation::AnimatorComponent> m_animator;

		/// <summary>
		/// The time spent in air before the platform falls.
		/// Necessary if the player is to jump off of the platform.
		/// </summary>
		const float m_fallDelay;
		float m_fallCounter = 0.f;
	};
}