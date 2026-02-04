#pragma once

#include "../../../Objects/GUI/StaticGUIElement.h"
#include "../../../Objects/GUI/Text/TextRenderer.h"

namespace GAME_NAME::Objects::GUI
{
	class AnimatingGUIElement
		: public StaticGUIElement
	{
	public:
		typedef std::vector<AnimatingGUIElement*> AnimatingNumber;

		static AnimatingNumber CreateAnimatingNumber(Text::TextRenderer::RenderedDigit& number, int layer = 1);

		AnimatingGUIElement(Vec2 position, Vec2 scale, Sprite* sprite);

		void Render(float zoom) override;
		
		/// <summary>
		/// Sets the target values for this current object to whatever its current values are.
		/// ex. TargetScale will be set to the current scale.
		/// </summary>
		void SetAnimationParamsToCurrent();

		void SetAnimationParams(Vec2 targetPosition, Vec2 targetScale, float targetOpacity = 1.f);
		/// <summary>
		/// Begin animating to the targets. Uses the current position, scale, and opacity as initial values.
		/// </summary>
		/// <param name="animationSpeed"></param>
		void StartAnimation(double animationSpeed);
		/// <summary>
		/// Begin animating to the targets.
		/// </summary>
		/// <param name="animationSpeed"></param>
		/// <param name="startPos"></param>
		/// <param name="startScale"></param>
		/// <param name="startOpacity"></param>
		void StartAnimation(double animationSpeed, Vec2 startPos, Vec2 startScale, float startOpacity = 1.f);

		inline void StopAnimation() { m_animationSpeed = -1.0; }

		inline void SetOpacity(float opacity) { m_opacity = opacity; }
	private:
		Vec2 m_targetPosition;
		Vec2 m_targetScale;

		float m_opacity = 1.f;
		float m_targetOpacity = 1.f;

		double m_animationSpeed = -1.0;
	};
}