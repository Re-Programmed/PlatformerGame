#include "GarageDoor.h"

#include "../../../../Utils/Time/GameTime.h"

#define GARAGE_DOOR_OPEN_SPEED 50.f

namespace GAME_NAME::Objects::Environment::Buildings
{
	GarageDoor::GarageDoor(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float maxOpenDistance, bool startOpen)
		: GameObject(position, scale, sprite), m_maxOpenDistance(maxOpenDistance), m_openDistance(startOpen ? m_maxOpenDistance : 0.f)
	{
		m_targetOpenDistance = m_openDistance;
	}

	void GarageDoor::Update(GLFWwindow* window)
	{
		float oldYPos = m_openDistance;

		if (m_openDistance != m_targetOpenDistance)
		{
			m_openDistance = std::lerp(m_openDistance, m_targetOpenDistance, static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()) * GARAGE_DOOR_OPEN_SPEED);
		}

		Translate(Vec2{ 0.f, m_openDistance - oldYPos });
	}

	void GarageDoor::Trigger(bool reverse, float intensity)
	{
		open(intensity * (reverse ? -1.f : 1.f));
	}

	void GarageDoor::open(float amount)
	{
		m_targetOpenDistance += amount;

		if (m_targetOpenDistance > m_maxOpenDistance) { m_targetOpenDistance = m_maxOpenDistance; }
		if (m_targetOpenDistance < 0.f) { m_targetOpenDistance = 0.f; }
	}

}
