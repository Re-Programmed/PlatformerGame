#pragma once

#include "./Cog.h"
#include "./Trigger.h"
#include "../Triggerable.h"


namespace GAME_NAME::Objects::Mechanical
{
	class TriggerCog
		: public Cog, public Trigger
	{
	public:
		TriggerCog(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, uint8_t gearRatio = 1, Triggerable* effect = nullptr);

		void Update(GLFWwindow* window) override;
	};
}