#pragma once

#include "../../../../Objects/GameObject.h"

namespace GAME_NAME::Objects::Environment::Buildings
{
	/// <summary>
	/// An object that becomes hidden when the player passes above it.
	/// </summary>
	class InnerUpperWall
		: public GameObject
	{
	public:
		InnerUpperWall(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float transitionOffset = 0.f, float transitionMaxOffset = 0.f);

		void Render(const Vec2& cameraPos) override;

	private:
		const float m_transitionOffset, m_transitionMaxOffset;
	};
}