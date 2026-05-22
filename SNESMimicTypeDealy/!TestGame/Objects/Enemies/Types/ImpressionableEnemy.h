#pragma once

#include "../Enemy.h"
#include "../../../../Components/Animation/AnimatorComponent.h"
#include "../../../Items/ItemType.h"


namespace GAME_NAME::Objects::Enemies
{
	class ImpressionableEnemy
		: public Enemy
	{
	public:
		enum class AttackType
		{
			Stabber = 0,
			Thrower = 1,
			Healer  = 2
		};

		ImpressionableEnemy(Vec2 position, AttackType type, bool initiallyAngry = false, size_t saveId = 0);

		void Update(GLFWwindow* window) override; 

		void Render(const Vec2& cameraPos) override;

		inline void SetHeldItem(Items::ITEM_TYPE item)
		{
			m_heldItem = item;
		}

	protected:
		Items::ITEM_TYPE m_heldItem;

		const AttackType m_attackType;

		/// <summary>
		/// Where the enemy spawned.
		/// </summary>
		const Vec2 m_home;
		
		bool m_isAngered = false;

		Components::Animation::AnimatorComponent* m_animator = nullptr;

		void updateMotion();
		void updateTextures();

		void attack();

	private:
		double m_attackCharge;
		double m_actionTimer;
	};
}