#pragma once

#include "../../../../Objects/GameObject.h"
#include "../../Triggerable.h"

namespace GAME_NAME::Objects::Environment::Buildings
{
	class GarageDoor
		: public GameObject, public Triggerable
	{
	public:
		GarageDoor(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float maxOpenDistance, bool startOpen = false);

		void Update(GLFWwindow* window) override;

		void Trigger(bool reverse, float intensity) override;

	protected:
		/// <summary>
		/// Increases the openess by the specified amount.
		/// </summary>
		/// <param name="amount"></param>
		void open(float amount);

	private:
		const float m_maxOpenDistance;
		float m_openDistance;
		float m_targetOpenDistance;
	};
}