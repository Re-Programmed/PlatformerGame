#include "HealthBarRenderer.h"

#include <iterator>

#include "../../../Rendering/Renderers/Renderer.h"

#define HEALTH_BAR_SCALE_X 16
#define HEALTH_BAR_SCALE_Y 2

#define HEALTH_BAR_Y_OFFSET 12

#define HEALTH_BAR_SPRITE_ID SpriteBase(42)
#define HEALTH_BAR_BACKDROP_ID SpriteBase(43)

namespace GAME_NAME::Objects::Enemies
{
typedef HealthBarRenderer::HealthBar HealthBar;

	std::unordered_map<GameObject*, HealthBar> HealthBarRenderer::m_createdHealthBars;

	void HealthBarRenderer::CreateHealthBar(GameObject* targetObject, float maxValue, float value)
	{
		//Check if bar already exists.
		for (auto& [object, bar] : m_createdHealthBars)
		{
			if (object == targetObject)
			{
				return;
			}
		}

		GameObject* healthBarObject = new GameObject(Vec2{ targetObject->GetPosition().X + targetObject->GetScale().X / 2.f, targetObject->GetPosition().Y + targetObject->GetScale().Y + HEALTH_BAR_Y_OFFSET }, { HEALTH_BAR_SCALE_X * (value / maxValue), HEALTH_BAR_SCALE_Y}, Rendering::Renderer::GetSprite(HEALTH_BAR_SPRITE_ID));
		//Increase scale by 2* healthBarObject size / pixels (ADJUST IF SPRITE CHANGES).
		GameObject* barBackdrop = new GameObject(healthBarObject->GetPosition(), healthBarObject->GetScale(), Renderer::GetSprite(HEALTH_BAR_BACKDROP_ID));

		Renderer::InstantiateObject(Renderer::InstantiateGameObject(barBackdrop, true, 1, true));
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(healthBarObject, true, 1, true));

		HealthBar bar{ healthBarObject, barBackdrop, value, maxValue };
		m_createdHealthBars.insert(std::make_pair(targetObject, bar));
	}

	void HealthBarRenderer::RemoveHealthBar(GameObject* const targetObject)
	{
		auto bar = m_createdHealthBars.find(targetObject);

		if (bar != m_createdHealthBars.end())
		{
			HealthBar& barData = bar->second;
			Renderer::DestroyActiveObject(barData.BarObject);
			Renderer::DestroyActiveObject(barData.BarBackdrop);
		
			m_createdHealthBars.erase(bar);
		}
	}

	void HealthBarRenderer::UpdateHealthBar(GameObject* targetObject, float value)
	{
		auto bar = m_createdHealthBars.find(targetObject);

		if (bar != m_createdHealthBars.end())
		{
			bar->second.Value = value;
			bar->second.BarObject->SetScale({ HEALTH_BAR_SCALE_X * (bar->second.Value / bar->second.MaxValue), HEALTH_BAR_SCALE_Y });

			const Vec2 position{ targetObject->GetPosition().X, targetObject->GetPosition().Y + (targetObject->GetScale().Y / 2.f) + HEALTH_BAR_Y_OFFSET };

			bar->second.BarObject->SetPosition(position);
			bar->second.BarBackdrop->SetPosition(position);
			 
		}
	}
}

