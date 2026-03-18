#pragma once

#include "../../../../Objects/GameObject.h"
#include "../../../../Components/Physics/Collision/Helpers/StaticBoxCollisionObject.h"
#include "../../Triggerable.h"

namespace GAME_NAME::Objects::Environment::Buildings
{
using namespace Components::Physics::Collision;

	class GarageDoor
		: public StaticBoxCollisionObject, public Triggerable
	{
	public:
		GarageDoor(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float maxOpenDistance, bool startOpen = false, bool hasCollision = true);

		void Update(GLFWwindow* window) override;

		void Render(const Vec2& cameraPos) override;

		void Trigger(bool reverse, float intensity) override;

	protected:
		/// <summary>
		/// Increases the openess by the specified amount.
		/// </summary>
		/// <param name="amount"></param>
		void open(float amount);

	private:
		const bool m_hasCollision;
		/// <summary>
		/// If this door does not have collision, it will hide the objects behind it until it is opened.
		/// </summary>
		std::vector<GameObject*> m_containedObjects;

		const float m_maxOpenDistance;
		float m_openDistance;
		float m_targetOpenDistance;

	};
}