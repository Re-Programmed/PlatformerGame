#pragma once

#include "../../Objects/GameObject.h"
#include "../../Rendering/Renderers/Renderer.h"

namespace GAME_NAME::Objects
{
	class LayerFlipObject
		: public GameObject
	{
	public:
		LayerFlipObject(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float flipOffset = 0.f)
			: GameObject(position, scale, sprite), m_flipOffset(flipOffset), m_frontDisplay(new GameObject(position, scale, nullptr))
		{ 
		
		}

		~LayerFlipObject()
		{
			if (m_frontDisplayed)
			{
				Renderer::DestroyActiveObjectImmediate(m_frontDisplay);
			}

			delete m_frontDisplay;
		}

		void Update(GLFWwindow* window) override;

		GameObject* const m_frontDisplay;
	private:
		bool m_frontDisplayed = false;

		const float m_flipOffset;
	};
}