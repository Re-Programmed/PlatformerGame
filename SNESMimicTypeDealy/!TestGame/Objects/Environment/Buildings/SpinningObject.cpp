#include "SpinningObject.h"
#include "../../../../Utils/Time/GameTime.h"
#include "../../../../MusicSync/MusicSync.h"
#include <corecrt_math_defines.h> //For M_PI

namespace GAME_NAME::Objects::Environment
{
	SpinningObject::SpinningObject(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float rotationSpeed, bool enabled)
		: GameObject(position, scale, sprite), m_rotationSpeed(enabled ? rotationSpeed : 0.f), m_targetSpeed(rotationSpeed), m_enabled(enabled)
	{

	}

	void SpinningObject::Update(GLFWwindow* window)
	{
		if (m_enabled && m_rotationSpeed != m_targetSpeed)
		{
			m_rotationSpeed = std::lerp(m_rotationSpeed, m_targetSpeed, 0.01f);
		}
		else if (!m_enabled && m_rotationSpeed != 0)
		{
			m_rotationSpeed = std::lerp(m_rotationSpeed, 0.f, 0.01f);
		}

		//Get how close we are to having (m_rotationSpeed) beats played through.
		double eTime = MusicSync::MusicSync::GetElapsedTimeWithinBeats((int)m_rotationSpeed);
		SetRotationAboutCenter(eTime * 360);
	}
}

