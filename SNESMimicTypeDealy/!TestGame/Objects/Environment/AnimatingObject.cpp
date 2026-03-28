#include "AnimatingObject.h"
#include <stdarg.h>

namespace GAME_NAME::Objects::Environment
{
	using namespace Components::Animation;

	AnimatingObject::AnimatingObject(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float animSpeed, size_t spriteCount, ...)
		: GameObject(position, scale, sprite), m_animator(nullptr)
	{
		va_list args;
		va_start(args, spriteCount);

		AnimData animData;
		
		for (size_t i = 0; i < spriteCount; i++)
		{
			animData.Sprites.push_back(std::shared_ptr<Sprite>(va_arg(i, Rendering::Sprite*)));
		}

		std::vector<std::shared_ptr<Animation>> animations;
		animations.push_back(std::make_shared<Animation>(animData, ANIM_12_SPF));
		m_animator = new AnimatorComponent(animations);
		m_animator->SetSpeedMult(animSpeed);

		m_animator->SetAllowLooping(true);
		m_animator->SetCurrentAnimation(0);
	}

	AnimatingObject::AnimatingObject(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float animSpeed, std::vector<Sprite*> sprites)
		: GameObject(position, scale, sprite), m_animator(nullptr)
	{
		AnimData animData;

		for (Sprite*& sprite : sprites)
		{
			animData.Sprites.push_back(std::shared_ptr<Sprite>(sprite));
		}

		std::vector<std::shared_ptr<Animation>> animations;
		animations.push_back(std::make_shared<Animation>(animData, ANIM_12_SPF));
		m_animator = new AnimatorComponent(animations);
		m_animator->SetSpeedMult(animSpeed);

		m_animator->SetAllowLooping(true);
		m_animator->SetCurrentAnimation(0);
	}


	void AnimatingObject::Update(GLFWwindow* window)
	{
		m_animator->Update(window, this);
	}
}

