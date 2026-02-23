#pragma once

#include "../Enemy.h"

namespace GAME_NAME::Objects::Enemies
{
	/// <summary>
	/// An enemy that jumps and walks randomly.
	/// Should be given a jumping sprite and one sprite directly after it for a second running animation.
	/// </summary>
	class PassiveRabbit
		: public Enemy
	{
	public:
		struct PassiveRabbitAttributes
			: public EnemyAttributes
		{
			float JumpHeight = 12.F;

			PassiveRabbitAttributes()
			{
				JumpHeight += ((std::rand() * 50.f) / RAND_MAX - 25.f) / 50.f;
			}
		};

		PassiveRabbit(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, int jumpingSprite, PassiveRabbitAttributes* attributes = new PassiveRabbitAttributes(), size_t saveID = 0);

		~PassiveRabbit();
		void Update(GLFWwindow* window) override;

		void onCollision(Vec2 push, GameObject* self, GameObject* other) override;

	protected:
		/// <summary>
		/// Updates the rabbit's textures to reflect its current action (running, jumping, etc.)
		/// </summary>
		/// <param name="window"></param>
		void updateTextures(GLFWwindow* window);

		/// <summary>
		/// Counts down to when the next action the rabbit will perform will occur.
		/// </summary>
		double m_actionTimer = 0.0;


	private:
		/// <summary>
		/// Stores the resting sprite of the rabbit.
		/// </summary>
		const int m_baseSprite;
		/// <summary>
		/// The index of the jumping and main running sprite.
		/// </summary>
		const int m_jumpingSprite;

		/// <summary>
		/// Used to determine what frame of animation the rabbit is on.
		/// (I'm not adding a whole animator to the rabbit, just this variable -- I could have added an animator component.)
		/// </summary>
		bool m_oddFrame = false;
		/// <summary>
		/// The last multiple of 8 that updated the rabbit's sprite. Used to prevent updating the rabbit's animation more than once on the same farme.
		/// </summary>
		int m_lastOddFrameX = 0;
	};
}