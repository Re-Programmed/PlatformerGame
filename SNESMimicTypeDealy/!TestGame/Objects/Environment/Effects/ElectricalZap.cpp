#include "ElectricalZap.h"
#include "../../../../Rendering/Renderers/Renderer.h"
#include "../../../../Utils/Time/GameTime.h"
#include "../../../../Utils/Math/VMath.h"

#define ELECTRICAL_ZAP_SPRITE_1 SpriteBase(251)
#define ELECTRICAL_ZAP_SPRITE_2 SpriteBase(252)

#define ELECTRICAL_ZAP_LIFETIME 0.5

namespace GAME_NAME::Objects::Environment::Effects
{

	std::map<GameObject*, double> ElectricalZap::m_zapEffects;
	ElectricalZap* ElectricalZap::INSTANCE = nullptr;

	void ElectricalZap::CreateZapBetweenPoints(Vec2 origin, Vec2 destination)
	{
		Vec2 scale = Vec2{ destination.X - origin.X, destination.Y - origin.Y };
		
		if (scale.X >= 0.f && scale.X < 6.f) 
		{
			scale.X = 6.f;
		} else if (scale.X < 0.f && scale.X > -6.f)
		{
			scale.X = -6.f;
		}

		if (scale.Y >= 0.f && scale.Y < 6.f)
		{
			scale.Y = 6.f;
		}
		else if (scale.Y < 0.f && scale.Y > -6.f)
		{
			scale.Y = -6.f;
		}
		
		float angle = Vec2::AngleBetween(origin, destination);

		GameObject* zapEffect = new GameObject(origin, scale, Renderer::GetSprite(ELECTRICAL_ZAP_SPRITE_1));
		zapEffect->SetRotationAboutCenter(MathUtils::to_degf(angle));
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(zapEffect, true, 1, true));

		m_zapEffects.emplace(zapEffect, 0);
	}

	double ZapTimer = 0.0;

	void ElectricalZap::Update(GLFWwindow* window)
	{
		if (m_zapEffects.size() == 0) { return; }

		ZapTimer += Utils::Time::GameTime::GetScaledDeltaTime();

		std::vector<GameObject*> toBeRemoved;
		for (auto& [object, lifetime] : m_zapEffects)
		{
			lifetime += Utils::Time::GameTime::GetScaledDeltaTime();

			if (ZapTimer > 0.1)
			{
				ZapTimer = 0.0;
				Sprite* sprite1 = Renderer::GetSprite(ELECTRICAL_ZAP_SPRITE_1);

				if (object->GetSprite()->GetSpriteId() == sprite1->GetSpriteId())
				{
					object->SetSprite(Renderer::GetSprite(ELECTRICAL_ZAP_SPRITE_2));
					delete sprite1;
				}
				else {
					object->SetSprite(sprite1);
				}
			}

			if (lifetime > ELECTRICAL_ZAP_LIFETIME)
			{
				toBeRemoved.push_back(object);
			}
		}


		for (GameObject* obj : toBeRemoved)
		{
			m_zapEffects.erase(obj);
			Renderer::DestroyActiveObject(obj);
		}
	}
}

