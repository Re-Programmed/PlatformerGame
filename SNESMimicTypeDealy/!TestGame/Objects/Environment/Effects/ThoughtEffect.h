#pragma once

#include "../../../../Objects/GameObject.h"
#include "../../../../Components/Animation/AnimatorComponent.h"
#include "../../../Items/ItemType.h"

namespace GAME_NAME::Objects::Environment::Effects
{
	/// <summary>
	/// Speech bubble with an item inside.
	/// </summary>
	class ThoughtEffect
		: public GameObject
	{
	public:
		ThoughtEffect(Vec2 position, GAME_NAME::Items::ITEM_TYPE itemDisplayed, double lifeSpan = 2.0);
		ThoughtEffect(Vec2 position, unsigned int sprietIDDisplayed, double lifeSpan = 2.0);

		//Should be used when creating a thought effect.
		static ThoughtEffect* CreateThoughtEffect(Vec2 position, GAME_NAME::Items::ITEM_TYPE itemDisplayed, double lifeSpan = 2.0);
		static ThoughtEffect* CreateThoughtEffect(Vec2 position, unsigned int spriteIDDisplayed, double lifeSpan = 2.0);

		void Update(GLFWwindow* window) override;
		void Render(const Vec2& cameraPos) override;

		~ThoughtEffect();

	private:
		Components::Animation::AnimatorComponent* m_animator;
		Sprite* m_displayedSprite;

		double m_lifeTimer = 0.0;

		const double m_lifeSpan;
	};
}