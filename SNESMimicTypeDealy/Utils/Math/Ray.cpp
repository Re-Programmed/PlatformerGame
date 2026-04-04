#include "Ray.h"

#include "../../Rendering/Renderers/Renderer.h"
#include "../../Components/Physics/Collision/Helpers/StaticBoxCollisionObject.h"

namespace GAME_NAME::MathUtils
{
    using namespace GAME_NAME::Objects;

    Vec2 Ray::m_continuousPoint(0.f);
    int Ray::m_maxIterations(0);
        
    //The area around the cast that is checked for objects and how quickly the cast moves forward.
#define RAY_CAST_TOLERENCE 10.f

    Objects::GameObject* Ray::CollisionCast(Vec2 origin, bool left, float travelDistance, Objects::GameObject* immune)
    {
        GameObject* closestObject(nullptr);
        float lastDist = travelDistance;

        auto objects = Rendering::Renderer::GetAllObjectsInArea(origin - Vec2{ left ? travelDistance : 0.f, RAY_CAST_TOLERENCE * 5.f / 2.f }, Vec2{ travelDistance, RAY_CAST_TOLERENCE * 5.f }, true);
        for (GameObject*& object : objects)
        {
            if (object == immune) { continue; }

            if (Components::Physics::Collision::StaticBoxCollisionObject* collider = dynamic_cast<Components::Physics::Collision::StaticBoxCollisionObject*>(object))
            {
                float dist(std::abs(object->GetPosition().X - origin.X));
                if (dist < travelDistance)
                {
                    closestObject = object;
                    lastDist = dist;
                }
            }
        }

        return closestObject;
    }

    GameObject* Ray::Cast(const Vec2 origin, Vec2 direction, Vec2& hitPoint, const float&& maxDistance, Objects::GameObject* immune)
    {
        //Tiny increments of the width of the checking area.
        direction.Normalize(RAY_CAST_TOLERENCE);

        Vec2 point = origin;

        int maxIterations = (int)(std::ceilf(maxDistance / direction.Length()));
        const Vec2 offset{ RAY_CAST_TOLERENCE/2.f };
        const Vec2 scale{ RAY_CAST_TOLERENCE };

        std::cout << maxIterations << "_iterations\n";
            
        for (int i = 0; i < maxIterations; i++)
        {
            

            //Loop over things that are hit.
            for(GameObject* object : Rendering::Renderer::GetAllObjectsInArea(point - offset, scale, true))
            {
                if (Components::Physics::Collision::StaticBoxCollisionObject* collider = dynamic_cast<Components::Physics::Collision::StaticBoxCollisionObject*>(object))
                {
                    //If it's the immune object, skip.
                    if (immune == object) { continue; }

                    //Otherwise we hit something.

                    hitPoint = point;
                    return object;
                }
            }

            point = point + direction;
        }

        hitPoint = Vec2{ 0.f };
        return nullptr;
    }

    bool Ray::ContinuousCast(const Vec2& origin, Vec2& direction, int& iterations, GameObject*& hit, const float&& maxDistance, Objects::GameObject* immune, const bool&& stopOnHit)
    {
        if (iterations == 0)
        {
            m_continuousPoint = origin;
            direction.Normalize(RAY_CAST_TOLERENCE);
            m_maxIterations = (int)(std::ceilf(maxDistance / direction.Length()));
        }
        else if (iterations >= m_maxIterations)
        {
            return false;
        }

        for (GameObject* object : Rendering::Renderer::GetAllObjectsInArea(m_continuousPoint - Vec2{ RAY_CAST_TOLERENCE/2.f }, Vec2{ RAY_CAST_TOLERENCE }, true))
        {
            //If it's the immune object, skip.
            if (immune == object) { continue; }

            //Otherwise we hit something.
            hit = object;

            if (stopOnHit) { return false; }

            break;
        }

        m_continuousPoint = m_continuousPoint + direction;
        iterations++;
        return true;
    }

}
