#include "GarageDoor.h"

#include "../../../../Utils/Time/GameTime.h"
#include "../../../../Rendering/Renderers/Renderer.h"
#include "../../../../Utils/CollisionDetection.h"

#define GARAGE_DOOR_OPEN_SPEED 32.f

namespace GAME_NAME::Objects::Environment::Buildings
{
	GarageDoor::GarageDoor(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, float maxOpenDistance, bool startOpen, bool hasCollision)
		: StaticBoxCollisionObject(position, scale, sprite), m_maxOpenDistance(maxOpenDistance), m_openDistance(startOpen ? m_maxOpenDistance : 0.f), m_hasCollision(hasCollision)
	{
		m_targetOpenDistance = m_openDistance;

		//Disable all objects hidden behind the door.
		if (!m_hasCollision)
		{
			for (GameObject* object : Rendering::Renderer::GetAllObjectsInArea(m_position, m_scale, false))
			{
				if (object == this) { continue; }

				m_containedObjects.emplace_back(object);
				object->SetActive(false);
			}
		}
	}

	void GarageDoor::Update(GLFWwindow* window)
	{
		float oldYPos = m_openDistance;

		if (m_openDistance != m_targetOpenDistance)
		{
			m_openDistance = std::lerp(m_openDistance, m_targetOpenDistance, static_cast<float>(Utils::Time::GameTime::GetScaledDeltaTime()) * GARAGE_DOOR_OPEN_SPEED);
		}

		Translate(Vec2{ 0.f, m_openDistance - oldYPos });

		if (m_hasCollision) 
		{
			StaticBoxCollisionObject::Update(window);
		}
	}

	void GarageDoor::Render(const Vec2& cameraPos)
	{
		if (m_openDistance > 0.f && m_openDistance < m_maxOpenDistance)
		{
			float alpha = (m_openDistance / m_maxOpenDistance);
			Vec4 bgColor[4] = {
				{ 1.f, 1.f, 1.f, alpha },
				{ 1.f, 1.f, 1.f, alpha },
				{ 1.f, 1.f, 1.f, alpha },
				{ 1.f, 1.f, 1.f, alpha }
			};

			for (GameObject* behind : m_containedObjects)
			{
				DynamicSprite fadedObject(behind->GetSprite()->GetSpriteId());
				fadedObject.UpdateTextureColor(bgColor);
				fadedObject.Render(cameraPos, behind->GetPosition() + behind->GetScale(), behind->GetScale() * -1.f);
			}

			DynamicSprite bgFade(Renderer::GetSpriteIdFromTextureId(SpriteBase(-1)));

			alpha = 1.f - alpha;
			Vec4 invBgColor[4] = {
				{ 1.f, 1.f, 1.f, alpha },
				{ 1.f, 1.f, 1.f, alpha },
				{ 1.f, 1.f, 1.f, alpha },
				{ 1.f, 1.f, 1.f, alpha }
			};

			bgFade.UpdateTextureColor(invBgColor);
			bgFade.Render(cameraPos, m_position - Vec2{ 0.f, m_openDistance }, m_scale);
		}

		StaticBoxCollisionObject::Render(cameraPos);
	}

#define GARAGE_DOOR_LOAD_OBJECTS_POSITION_LIMIT 0.9f

	void GarageDoor::Trigger(bool reverse, float intensity)
	{
		open(intensity * (reverse ? -1.f : 1.f));

		//Eanble/Disable objects behind.
		if (!m_hasCollision && m_openDistance < m_maxOpenDistance)
		{
			if (m_openDistance > (GARAGE_DOOR_LOAD_OBJECTS_POSITION_LIMIT * m_maxOpenDistance))
			{
				for (GameObject* object : m_containedObjects)
				{
					object->SetActive(true);
				}
			}
			else if (m_openDistance > 0.f)
			{
				for (int i = 0; i < m_containedObjects.size(); i++)
				{
					GameObject* object = m_containedObjects[i];
					//If the object is no longer behind the door, don't hide it.
					if (Utils::CollisionDetection::PointWithinBoxBL(object->GetPosition() + object->GetScale() / 2.f, m_position - Vec2{ 0.f, m_openDistance }, m_scale))
					{
						object->SetActive(false);
					}
					else {
						m_containedObjects.erase(m_containedObjects.begin() + (i--));
					}
				}
			}
		}

	}

	void GarageDoor::open(float amount)
	{
		m_targetOpenDistance += amount;

		if (m_targetOpenDistance > m_maxOpenDistance) { m_targetOpenDistance = m_maxOpenDistance; }
		if (m_targetOpenDistance < 0.f) { m_targetOpenDistance = 0.f; }
	}

}
