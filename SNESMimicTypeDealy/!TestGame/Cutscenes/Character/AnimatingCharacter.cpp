#include "AnimatingCharacter.h"

#include "../../../Utils/Time/GameTime.h"

namespace GAME_NAME::Cutscenes
{

	AnimatingCharacter::AnimatingCharacter(Vec2 position, Vec2 scale, bool gravityObserved, float stopDistance, Player::Player::PlayerTextureData* textureData)
		: ActiveBoxCollisionGravityObject(position, scale, Renderer::GetSprite(textureData->DefaultSprites)), m_target{ /*Target Point: */position, /*Target Object: */nullptr }, m_gravityObserved(gravityObserved), m_stopDistance(stopDistance),
		m_textureData(textureData)
	{
		if (gravityObserved)
		{
			m_physics->SetGravityStrength(DefaultCharacterGravity);
		}
		else {
			m_physics->SetGravityStrength(0.f);
		}

		registerAnimations();
	}

	void AnimatingCharacter::Update(GLFWwindow* window)
	{
		if (m_target.Object != nullptr)
		{
			m_target.Location = m_target.Object->GetPosition() + m_target.Object->GetScale() / 2.f;
		}

		pathfind();
		updateAnimations();

		m_animator->Update(window, this);
		ActiveBoxCollisionGravityObject::Update(window);
	}

	void AnimatingCharacter::Render(const Vec2& cameraPos)
	{
		m_didRender = true;

		//DEFAULT RENDER MODE.
		m_sprite->Render(cameraPos, m_position + (m_textureFlipped ? (m_scale * Vec2::OneX) : 0), m_scale * (m_textureFlipped ? Vec2::MinusOneXOneY : 1), m_rotation);
	}

	void AnimatingCharacter::SetTarget(Vec2 targetLocation)
	{
		m_target.Location = targetLocation;
		m_target.Object = nullptr;
	}

	void AnimatingCharacter::SetTarget(GameObject* object)
	{
		m_target.Object = object;
		m_target.Location = object->GetPosition() + object->GetScale()/2.f;
	}

	void AnimatingCharacter::Jump()
	{
		if (m_onGround && m_physics->GetVelocity().Y < DefaultCharacterJumpHeight)
		{
			m_physics->AddVelocity(Vec2{ 0.f, DefaultCharacterJumpHeight });
		}
	}

	void AnimatingCharacter::beforeCollision()
	{
		if (m_gravityObserved && m_physics->GetGravitationalVelocity() < -m_physics->GetGravityStrength() - 1)
		{
			m_onGround = false;
		}
	}

	void AnimatingCharacter::onCollision(Vec2 push, GameObject* self, GameObject* other)
	{
		if (push.Y > 0)
		{
			this->m_physics->SetVelocityY(0.f);

			m_onGround = true;
		}
	}

	void AnimatingCharacter::pathfind()
	{
		if (m_gravityObserved)
		{
			//Right edge is not stop distance away from target.
			if (m_position.X + m_scale.X < m_target.Location.X - m_stopDistance && this->m_physics->GetVelocity().X < m_speedCap)
			{
				this->m_physics->AddVelocity(Vec2(((float)Utils::Time::GameTime::GetScaledDeltaTime() / 0.017f) * (m_speedCap - m_physics->GetVelocity().X) * (m_speed), 0.f));
				m_textureFlipped = true;

				if (this->GetVelocity().X < 0.f)
				{
					//Skidding.
					this->m_physics->SetFrictionDrag(3.f);
				}
				else {
					this->m_physics->SetFrictionDrag(0.f);
				}
			}
			//Left edge is not stop distance away from target (with width of target respected).
			else if (m_position.X > m_target.Location.X + m_stopDistance + (m_target.Object == nullptr ? 0.f : m_target.Object->GetScale().X) && -this->m_physics->GetVelocity().X < m_speedCap)
			{
				this->m_physics->AddVelocity(Vec2(((float)Utils::Time::GameTime::GetScaledDeltaTime() / 0.017f) * (m_speedCap - m_physics->GetVelocity().X) * (-(m_speed - 0.0165F)), 0.f));
				this->m_physics->SetFrictionDrag(0.f);
				m_textureFlipped = false;

				if (this->GetVelocity().X > 0.f)
				{
					//Skidding.
					this->m_physics->SetFrictionDrag(3.f);
				}
				else {
					this->m_physics->SetFrictionDrag(0.f);
				}
			}
			else {
				this->m_physics->SetFrictionDrag(DefaultCharacterFriction);
			}

			if (m_target.Location.Y - 60.f > m_position.Y)
			{
				Jump();
			}
		}
	}

	void AnimatingCharacter::updateAnimations()
	{
		float animMomentum = std::abs(m_physics->GetVelocity().X);

		if (!m_onGround)
		{
			float yVel = m_physics->GetVelocity().Y + m_physics->GetGravitationalVelocity();

			if (yVel < 0.f)
			{
				m_animator->SetCurrentAnimation(3); //Jumping Animation
				return;
			}

			if (yVel > 0.f)
			{
				m_animator->SetCurrentAnimation(2); //Falling Animation
				return;
			}
		}else if (animMomentum > 0.1f)	//Check if character is moving a certain speed and is on the ground.
		{
			m_timeSpentNotMoving = 0.0;
			m_animator->SetSpeedMult((double)animMomentum / 100.0);
			if (animMomentum > m_speedCap - 18.f)
			{
				m_animator->SetCurrentAnimation(1);  //Running Animation
			}
			else {
				m_animator->SetCurrentAnimation(0);  //Walking Animation
			}

			return;
		}

		if(this->m_onGround) 
		{
			//Default standing sprite.
			m_animator->SetCurrentAnimation(-1);
			m_sprite.reset(Renderer::GetSprite(m_textureData->DefaultSprites));
			return;
		}
	}

	void AnimatingCharacter::registerAnimations()
	{
using namespace Components::Animation;

#define RegisterAnimation(data_name, data_out, baseRef) AnimData data_name;									\
				speed = 0.f;																								\
					for(int i = 0; i < m_textureData->AnimationOverride->data_out.size; i++)																\
					{																																	\
						data_name.Sprites.emplace_back(std::shared_ptr<Sprite>(Renderer::GetSprite(m_textureData->AnimationOverride->data_out.anim[i] + baseRef)));\
					}																											\
				if (m_textureData->AnimationOverride->data_out.speed > 0) { speed = m_textureData->AnimationOverride->data_out.speed; } \
																															\
				std::shared_ptr<GAME_NAME::Components::Animation::Animation> data_out(new GAME_NAME::Components::Animation::Animation(data_name, speed)); 
		
		float speed;

		RegisterAnimation(walk_data, walk_anim, m_textureData->DefaultSprites);						  //0
		RegisterAnimation(run_data, run_anim, m_textureData->DefaultSprites);						  //1
		RegisterAnimation(fall_data, fall_anim, m_textureData->DefaultSprites);						  //2
		RegisterAnimation(jump_data, jump_anim, m_textureData->DefaultSprites);						  //3

		std::vector<std::shared_ptr<GAME_NAME::Components::Animation::Animation>> anims{ walk_anim, run_anim, fall_anim, jump_anim };

		m_animator = new AnimatorComponent(anims);
	}
}
