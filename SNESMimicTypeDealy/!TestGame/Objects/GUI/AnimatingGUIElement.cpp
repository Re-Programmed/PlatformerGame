#include "AnimatingGUIElement.h"

namespace GAME_NAME::Objects::GUI
{
	AnimatingGUIElement::AnimatingNumber AnimatingGUIElement::CreateAnimatingNumber(Text::TextRenderer::RenderedDigit& number, int layer)
	{
		AnimatingNumber animNumber;

		for (StaticGUIElement* digit : number)
		{
			AnimatingGUIElement* animatingVersion = new AnimatingGUIElement(digit->GetPosition(), digit->GetScale(), new Sprite(digit->GetSprite()->GetSpriteId()));
			Renderer::UnloadGUIElement(digit, layer);
			delete digit;

			Renderer::LoadGUIElement(animatingVersion, layer);
			animNumber.emplace_back(animatingVersion);
		}

		number.clear();

		return animNumber;
	}

	AnimatingGUIElement::AnimatingGUIElement(Vec2 position, Vec2 scale, Sprite* sprite)
		: StaticGUIElement(position, scale, sprite), m_targetPosition(position), m_targetScale(scale)
	{

	}

	void AnimatingGUIElement::Render(float zoom)
	{
		if (!m_active) { return; }

		if (m_animationSpeed > 0.f)
		{
			m_position = Vec2::Lerp(m_position, m_targetPosition, m_animationSpeed);
			m_scale = Vec2::Lerp(m_scale, m_targetScale, m_animationSpeed);
			m_opacity = std::lerp(m_opacity, m_targetOpacity, m_animationSpeed);
		}

		//Create dynamic sprite to allow for opacity.
		DynamicSprite sprite(m_sprite->GetSpriteId());

		Vec4 color[4] = {
			{ 1.f, 1.f, 1.f, m_opacity },
			{ 1.f, 1.f, 1.f, m_opacity },
			{ 1.f, 1.f, 1.f, m_opacity },
			{ 1.f, 1.f, 1.f, m_opacity }
		};

		sprite.UpdateTextureColor(color);

		sprite.Render(Vec2::Zero, (m_position + m_scale) / zoom, (m_scale * Vec2{ -1.f }) / zoom, 0.0f);
	}

	inline void AnimatingGUIElement::SetAnimationParams(Vec2 targetPosition, Vec2 targetScale, float targetOpacity)
	{
		m_targetPosition = targetPosition;
		m_targetScale = targetScale;
		m_targetOpacity = targetOpacity;
	}

	void AnimatingGUIElement::StartAnimation(double animationSpeed)
	{
		m_animationSpeed = animationSpeed;
	}

	void AnimatingGUIElement::SetAnimationParamsToCurrent()
	{
		m_targetPosition = m_position;
		m_targetScale = m_scale;
		m_targetOpacity = m_opacity;
	}

	void AnimatingGUIElement::StartAnimation(double animationSpeed, Vec2 startPos, Vec2 startScale, float startOpacity)
	{
		m_animationSpeed = animationSpeed;

		m_position = startPos;
		m_scale = startScale;
	}

}
