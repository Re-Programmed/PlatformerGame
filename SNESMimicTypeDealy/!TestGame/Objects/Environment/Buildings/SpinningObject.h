#pragma once
#include "../../../../Objects/GameObject.h"

namespace GAME_NAME::Objects::Environment
{
	class SpinningObject
		: public GameObject
	{
	public:
		SpinningObject(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float rotationSpeed, bool enabled = true);

		void Update(GLFWwindow* window) override;

		inline void SetTargetSpeed(float targetSpeed)
		{
			m_targetSpeed = targetSpeed;
		}

		inline void SetEnabled(bool enabled)
		{
			m_enabled = enabled;
		}

		inline const float& GetRotationSpeed()
		{
			return m_rotationSpeed;
		}

	private:
		/// <summary>
		/// The current actual speed that the object is spinning at.
		/// </summary>
		float m_rotationSpeed;
		/// <summary>
		/// The speed the spinning object will move at when enabled.
		/// </summary>
		float m_targetSpeed;

		/// <summary>
		/// If false, the object will slow to 0 and stop spinning.
		/// </summary>
		bool m_enabled;
	};
}