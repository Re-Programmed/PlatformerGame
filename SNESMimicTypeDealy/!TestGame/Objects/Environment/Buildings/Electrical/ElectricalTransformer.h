#pragma once

#include "../../../../../Objects/GameObject.h"

#define ELECTRICAL_TRANSFORMER_ZAP_COOLDOWN 0.75
#define ELECTRICAL_TRANSFORMER_SUPERCHARGE_AMOUNT 1

namespace GAME_NAME::Objects::Environment::Buildings
{
	class ElectricalTransformer
		: public GameObject
	{
	public:

		ElectricalTransformer(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float zapRadius)
			: GameObject(position, scale, sprite), m_zapRadius(zapRadius)
		{

		}

		void Update(GLFWwindow* window) override;

	private:
		const float m_zapRadius;
		double m_cooldown = 0.0;
		double m_randomZapTimer = 0.0;
	};
}