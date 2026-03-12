#pragma once

#include "LeftRightEnemy.h"

#include "../../../../Components/Animation/AnimatorComponent.h"

namespace GAME_NAME::Objects::Enemies
{
	class AngryFarmer
		: public LeftRightEnemy
	{
	public:
		struct AngryFarmerAttributes
			: public LeftRightEnemyAttributes
		{
			

			AngryFarmerAttributes() = default;
		};

		void Update(GLFWwindow* window) override;

		AngryFarmer(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, Vec2 leftAnchor, Vec2 rightAnchor, AngryFarmerAttributes* attributes = new AngryFarmerAttributes(), size_t saveID = 0);
		
		~AngryFarmer();

	protected:
		void lunge();
		void calculateAttack();

		void updateTextures();

		void onCollision(Vec2 push, GameObject* self, GameObject* other) override;
	private:
		double m_attacking = 0.0;
		bool m_hasHitPlayerThisAttack = false;

		GAME_NAME::Components::Animation::AnimatorComponent* m_animatorComponent;

		/// <summary>
		/// True if the farmer has spotted the player in his vision. Causes the farmer to begin chasing the player.
		/// </summary>
		bool m_isAggrivated = false;
	};
}