#include "JointSpinPlatform.h"
#include "../../TestGame.h"
#include "../../../Utils/Time/GameTime.h"

#define JOINT_SPIN_PLATFORM_FB_ANIMATION_COUNT 7

using namespace GAME_NAME::Components::Animation;

namespace GAME_NAME::Objects::Platforms
{

	JointSpinPlatform::JointSpinPlatform(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, double fallTime, bool FB)
		: StaticBoxCollisionObject(position, scale, sprite), m_isForwardBackward(FB), m_fallDelay(fallTime)
	{
		SetOnCollision(onCollision);

		if (!FB) { return; }

#pragma region AnimationInitilization
		std::vector<std::shared_ptr<Components::Animation::Animation>> animations;

		std::vector<std::shared_ptr<Sprite>> spritesForward;
		for (int i = 1; i < JOINT_SPIN_PLATFORM_FB_ANIMATION_COUNT; i++)
		{
			spritesForward.push_back(std::make_shared<Sprite>(m_sprite->GetSpriteId() + i));
		}

		std::vector<std::shared_ptr<Sprite>> spritesBackward;
		for (int i = JOINT_SPIN_PLATFORM_FB_ANIMATION_COUNT - 1; i >= 0; i--)
		{
			spritesBackward.push_back(std::make_shared<Sprite>(m_sprite->GetSpriteId() + i));
		}

		animations.push_back(std::make_shared<Components::Animation::Animation>(AnimData(spritesForward), ANIM_6_SPF));
		animations.push_back(std::make_shared<Components::Animation::Animation>(AnimData(spritesBackward), ANIM_6_SPF));

		m_animator = std::make_unique<AnimatorComponent>(animations);
		m_animator->SetAllowLooping(false);
#pragma endregion
	}

	bool JointSpinPlatform::onCollision(Vec2 push, Objects::GameObject* object, Objects::GameObject* collider)
	{
		JointSpinPlatform* jsp = static_cast<JointSpinPlatform*>(object);

		//No collision, the platform is in fallen state.
		if (jsp->m_hasFallen)
		{
			return false;
		}

		//Landed on top.
		if (push.Y > 0)
		{
			jsp->m_fallCounter += 0.001f;

			return jsp->m_fallCounter < jsp->m_fallDelay;
		}

		//Collision was not from top, ignore.
		return false;
	}

	void JointSpinPlatform::Update(GLFWwindow* window)
	{
		StaticBoxCollisionObject::Update(window);
		//Could be if it's not FB.
		if(m_animator != nullptr){ m_animator->Update(window, this); }

		if (m_fallCounter >= m_fallDelay)
		{
			if (m_hasFallen)
			{
				if (m_fallCounter > 10 * m_fallDelay)
				{
					m_fallCounter = 0;
					m_hasFallen = false;

					if (m_isForwardBackward)
					{
						m_animator->SetCurrentAnimation(1);
					}
					else {
						if (m_rotation < 180.f)
						{
							RotateAboutCenter(Utils::Time::GameTime::GetScaledDeltaTime() * 24.f);
						}
					}
				}
			}
			else {
				m_hasFallen = true;

				if (m_isForwardBackward)
				{
					if (m_animator->GetCurrentAnimationIndex() != 0)
					{
						m_animator->SetCurrentAnimation(0);
						m_fallCounter = m_fallDelay + 0.1f;
					}
				}
				else {
					if (m_rotation > 0.f)
					{
						RotateAboutCenter(-Utils::Time::GameTime::GetScaledDeltaTime() * 24.f);
					}
				}
			}
		}
		
		if (m_fallCounter > 0)
		{
			m_fallCounter += (float)Utils::Time::GameTime::GetScaledDeltaTime();
		}
	}


}
