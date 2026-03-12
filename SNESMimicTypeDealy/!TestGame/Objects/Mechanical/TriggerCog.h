#pragma once

#include "./Cog.h"
#include "../Triggerable.h"


namespace GAME_NAME::Objects::Mechanical
{
	class TriggerCog
		: public Cog
	{
	public:
		TriggerCog(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, uint8_t gearRatio = 1, Triggerable* effect = nullptr);

		inline void SetEffect(Triggerable* effect) { m_effect = effect; }

		void Update(GLFWwindow* window) override;

	private:
		Triggerable* m_effect;
	};
}