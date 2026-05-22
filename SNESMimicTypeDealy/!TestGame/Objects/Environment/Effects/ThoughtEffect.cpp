#include "ThoughtEffect.h"

#include "../../../../Utils/Time/GameTime.h"

#define THOUGHT_BUBBLE_SCALE Vec2{12.f, 12.f}
#define THOUGHT_BUBBLE_SPRITE SpriteBase(487)

namespace GAME_NAME::Objects::Environment::Effects
{
	ThoughtEffect::ThoughtEffect(Vec2 position, GAME_NAME::Items::ITEM_TYPE itemDisplayed, double lifeSpan)
		: GameObject(position, THOUGHT_BUBBLE_SCALE, nullptr), m_lifeSpan(lifeSpan),
		m_animator(nullptr)
	{
		Sprite* itemSprite = GAME_NAME::Items::ITEMTYPE_GetItemTypeTexture(itemDisplayed);
		ThoughtEffect::ThoughtEffect(position, itemSprite->GetSpriteId(), lifeSpan);
		delete itemSprite;
	}

	ThoughtEffect::ThoughtEffect(Vec2 position, unsigned int spriteIDDisplayed, double lifeSpan)
		: GameObject(position, THOUGHT_BUBBLE_SCALE, nullptr), m_lifeSpan(lifeSpan),
		m_animator(nullptr), m_displayedSprite(new Sprite(spriteIDDisplayed))
	{
		std::vector<std::shared_ptr<Sprite>> anim1;

		anim1.emplace_back(Renderer::GetSprite(THOUGHT_BUBBLE_SPRITE));
		anim1.emplace_back(Renderer::GetSprite(THOUGHT_BUBBLE_SPRITE + 1));

		std::vector<std::shared_ptr<Components::Animation::Animation>> animations{ std::make_shared<Components::Animation::Animation>(Components::Animation::AnimData{ anim1 }, ANIM_12_SPF) };

		m_animator = new Components::Animation::AnimatorComponent(animations);
		m_animator->SetCurrentAnimation(0, this);
	}

	ThoughtEffect* ThoughtEffect::CreateThoughtEffect(Vec2 position, GAME_NAME::Items::ITEM_TYPE itemDisplayed, double lifeSpan)
	{
		Sprite* sprite = Items::ITEMTYPE_GetItemTypeTexture(itemDisplayed);
		ThoughtEffect* thought = CreateThoughtEffect(position, sprite->GetSpriteId(), lifeSpan);
		delete sprite;
		return thought;
	}

	ThoughtEffect* ThoughtEffect::CreateThoughtEffect(Vec2 position, unsigned int spriteIDDisplayed, double lifeSpan)
	{
		ThoughtEffect* thought = new ThoughtEffect(position, spriteIDDisplayed, lifeSpan);
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(thought, true, 2, true));
		return thought;
	}

	ThoughtEffect::~ThoughtEffect()
	{
		delete m_animator;
		delete m_displayedSprite;
	}


	void ThoughtEffect::Update(GLFWwindow* window)
	{
		m_animator->Update(window, this);

		m_lifeTimer += Utils::Time::GameTime::GetScaledDeltaTime();
		if (m_lifeTimer >= m_lifeSpan)
		{
			Renderer::DestroyActiveObject(this);
		}
	}

	void ThoughtEffect::Render(const Vec2& cameraPos)
	{
		GameObject::Render(cameraPos);
		
		m_displayedSprite->Render(cameraPos, m_position + Vec2{ THOUGHT_BUBBLE_SCALE/8.f }, Vec2{ THOUGHT_BUBBLE_SCALE/4.f });
	}

}
