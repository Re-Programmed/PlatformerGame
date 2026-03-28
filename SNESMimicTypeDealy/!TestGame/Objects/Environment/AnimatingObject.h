#pragma once

#include "../../../Objects/GameObject.h"
#include "../../../Components/Animation/AnimatorComponent.h"

namespace GAME_NAME::Objects::Environment
{
	class AnimatingObject
		: public GameObject
	{
	public:
		AnimatingObject(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float animSpeed, size_t spriteCount, ...);
		AnimatingObject(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float animSpeed, std::vector<Rendering::Sprite*> sprites);

		void Update(GLFWwindow* window) override;

	protected:
		Components::Animation::AnimatorComponent* m_animator;

	private:
	};
}