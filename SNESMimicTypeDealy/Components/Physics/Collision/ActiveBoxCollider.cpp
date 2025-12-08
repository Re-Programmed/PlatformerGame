#include "ActiveBoxCollider.h"
#include "../../../Utils/CollisionDetection.h"
#include "PixelPerfectCollider.h"

#define _DEBUG_DISPLAY_COLLISION_BOUNDS false

#if _DEBUG_DISPLAY_COLLISION_BOUNDS
#include "../../../Rendering/DynamicSprite.h"
#include "../../../!TestGame/TestGame.h"
GameObject* m_renderDisplay = nullptr;
GameObject* m_renderDisplayCollided = nullptr;
#endif

//The maximum/minimum height that objects can be pushed on top of other objects when colliding with the side of each other.
#define DAMPING_STEP_HEIGHT_MAX 4.1f		
#define DAMPING_STEP_HEIGHT_MIN 0.1f
#define STEP_X_PUSH_MIN 0.05f

#define GetObjectW GetObject

namespace GAME_NAME
{
	namespace Components
	{
		namespace Physics
		{
			namespace Collision
			{
				void ActiveBoxCollider::Update(GLFWwindow* window, Objects::GameObject* object)
				{

				}

				void ActiveBoxCollider::CheckStaticCollisionShapes(StaticCollider* collider)
				{
					//Detection for box hitting box.
					if (instanceof<StaticBoxCollider>(collider))
					{
						Vec2 objectBounds = m_object->GetScale();
						Vec2 objectPos = m_object->GetPosition();

						/*
						ActiveBoxCollisionGravityObject* phys = dynamic_cast<ActiveBoxCollisionGravityObject*>(m_object);

						if (phys != nullptr)
						{
							Vec2 pVel = phys->GetVelocity() / 25.f;

							if (pVel.X < 0)
							{
								objectPos.X += pVel.X;
								objectBounds.X -= pVel.X;
							}
							else {
								objectBounds.X += pVel.X;
							}

							if (pVel.Y < 0)
							{
								objectPos.Y += pVel.Y;
								objectBounds.Y -= pVel.Y;
							}
							else {
								objectBounds.Y += pVel.Y;
							}
						}
						*/
#if _DEBUG_DISPLAY_COLLISION_BOUNDS
						if (m_renderDisplay == nullptr)
						{
							m_renderDisplay = new GameObject(objectPos, objectBounds, Renderer::GetSprite(SpriteBase(-1)));
							Renderer::LoadActiveObject(m_renderDisplay);
						}
						else {
							m_renderDisplay->SetPosition(objectPos);
							m_renderDisplay->SetScale(objectBounds);
						}
#endif
						Vec2 push = Vec2::Zero;
						if (Utils::CollisionDetection::BoxWithinBox(objectPos, objectBounds, collider->GetObject()->GetPosition(), collider->GetObject()->GetScale(), push))
						{

							//Check if the static collider has anything that needs to happen when it collides.
							if (collider->OnCollision != nullptr)
							{
								if (!(collider->OnCollision)(push, collider->GetObject(), m_object))
								{
									return;
								}
							}

							if (this->m_allowStep)
							{
								//Allow active objects colliding with the side of another static object to be pushed on top of the static object if they're close enough to the top edge of the static object.
								float topEdgeDistance = collider->GetObject()->GetPosition().Y + collider->GetObject()->GetScale().Y - objectPos.Y;
								if (topEdgeDistance < DAMPING_STEP_HEIGHT_MAX && topEdgeDistance > DAMPING_STEP_HEIGHT_MIN && (push.X > STEP_X_PUSH_MIN || push.X < -STEP_X_PUSH_MIN))
								{
									push.Y += topEdgeDistance;
									push.X = 0;
								}
							}

							
							//Allow the player to jump through small platforms from the bottom.
							if (push.Y < 0)
							{
								if (collider->GetObject()->GetScale().Y <= 5)
								{
									push.Y = 0;
								}
							}

							//Push the objects apart.
							m_object->Translate(push);

							//Check if the active collider has anything that needs to happen when it collides.
							if (m_onCollision != nullptr)
							{
								(m_onCollision)(push, m_object);
							}

#if _DEBUG_DISPLAY_COLLISION_BOUNDS
							if (m_renderDisplayCollided == nullptr)
							{
								m_renderDisplayCollided = new GameObject(collider->GetObject()->GetPosition(), collider->GetObject()->GetScale(), Renderer::GetSprite(SpriteBase(-1)));
								Renderer::LoadActiveObject(m_renderDisplayCollided);
							}
							else {
								m_renderDisplayCollided->SetPosition(collider->GetObject()->GetPosition());
								m_renderDisplayCollided->SetScale(collider->GetObject()->GetScale());
							}
#endif
						}
					}
				}

				void ActiveBoxCollider::CheckActiveCollisionShapes(ActiveCollider* collider)
				{

				}
			}
		}
	}
}