#pragma once

#include "../../../Objects/GameObject.h"

namespace GAME_NAME::Objects::Environment
{
	class Spikes
		: public GameObject
	{
	public:
		/// <summary>
		/// Create a set of spikes. Facing can be: 0 - up, 1 - left, 2 - down, 3 - right
		/// </summary>
		Spikes(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float damage, uint8_t facing = 0)
			: GameObject(position, scale, sprite), m_damage(damage), m_facing(facing)
		{
			
		}

		void Update(GLFWwindow* window) override;

	private:
		bool m_hasBeenHit = false;

		const float m_damage;
		const uint8_t m_facing;
	};
}