#pragma once

#include "../../../Objects/Helpers/Interactable.h"

namespace GAME_NAME::Objects::Environment
{
	class ClimbableObject
		: public Interactable
	{
	public:

		/// <summary>
		/// Creates an climbable object.
		/// </summary>
		/// <param name="position"> - where it is.</param>
		/// <param name="scale"> - the scale of the object.</param>
		/// <param name="sprite"> - the sprite.</param>
		ClimbableObject(Vec2 position, Vec2 scale, Rendering::Sprite* sprite);

		void Render(const Vec2& cameraPosition) override;
	private:
		void onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state) override;
	};
}