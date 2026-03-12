#include "TriggerCog.h"

namespace GAME_NAME::Objects::Mechanical
{
	TriggerCog::TriggerCog(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, uint8_t gearRatio, Triggerable* effect)
		: Cog(position, scale, sprite, false, gearRatio, 0.f, false), m_effect(effect)
	{

	}

	void TriggerCog::Update(GLFWwindow* window)
	{
		Cog::Update(window);

		if (m_effect == nullptr) { return; }

		if (m_gearSpeed > 0.f)
		{
			m_effect->Trigger(m_counterClockwise, m_gearSpeed/33.f);
		}
	}
}
