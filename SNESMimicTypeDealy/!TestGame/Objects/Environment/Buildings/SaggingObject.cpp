#include "SaggingObject.h"

#include "../../../../Utils/CollisionDetection.h"
#include "../../../../Rendering/DynamicSprite.h"
#include "../../../../Components/Physics/Collision/Helpers/ActiveBoxCollisionGravityObject.h"

namespace GAME_NAME::Objects::Environment::Buildings
{
	//Lists that show the ratio of the size of each segment based on how many there are.
	constexpr float SAG_SEGMENTS_3[3] = { 2.5f, 5.f, 2.5f };
	constexpr float SAG_SEGMENTS_5[5] = { 1.f, 2.f, 4.f, 2.f, 1.f };
	constexpr float SAG_SEGMENTS_7[7] = { 0.5f, 1.f, 2.f, 3.f, 2.f, 1.f, 0.5f };

	
	SaggingObject::SaggingObject(Vec2 position, Vec2 scale, Sprite* sprite, uint8_t sagSegments, bool bouncy)
		: GameObject(position, scale, sprite), m_isBouncy(bouncy)
	{
		//Limit segment count to the defined arrays above.
		if (sagSegments % 2 == 0) { sagSegments++; }
		if (sagSegments < 3) { sagSegments = 3; }
		if (sagSegments > 7) { sagSegments = 7; }

		//Create objects for each segment with respective size and position.
		float currentOffset = 0.f;
		for (int i = 0; i < sagSegments; i++)
		{
			float scaleX = 0.f;
			switch (sagSegments)
			{
			case 3:
				scaleX = SAG_SEGMENTS_3[i] * (m_scale.X/10.f);
				break;
			case 5:
				scaleX = SAG_SEGMENTS_5[i] * (m_scale.X / 10.f);
				break;
			case 7:
			default:
				scaleX = SAG_SEGMENTS_7[i] * (m_scale.X / 10.f);
				break;
			}

			//Create a new sagging object with respective position and scale.
			//The last parameter is the previous object.
			SaggingSegment* sagObject = new SaggingSegment(m_position + Vec2{ currentOffset, 0.f }, Vec2{ scaleX, m_scale.Y }, sprite, ((i > 0) ? m_sagObjects[i - 1] : nullptr));
			if (i > 0) { m_sagObjects[i - 1]->RightSag = sagObject; }

			//Add events that will help determine when an object should create a sag point.
			sagObject->SetOnCollision(saggingObjectCollisionEvent);
			sagObject->SetBeforeCollision(saggingObjectBeforeCollision);

			m_sagObjects.push_back(sagObject);
			currentOffset += scaleX;
		}
	}

	void SaggingObject::Update(GLFWwindow* window)
	{
		//This loop ensures that only one of the sagging objects has its "PreventUnsag" pointer as a value. This means that only one part of the sagging object will ever be checking that the player left its bounds -- which triggers an update to the sag shape.
		//The loop determines which segment should restrict updates based on which segment is sagging the most.
		int maintainSag = -1; float sagAmnt = 0.f;
		for (int i = 0; i < m_sagObjects.size(); i++)
		{
			if (m_sagObjects[i]->PreventUnsag && m_sagObjects[i]->PreviousSag <= sagAmnt)
			{
				if (maintainSag != -1 && m_sagObjects[maintainSag]->PreventUnsag)
				{
					m_sagObjects[maintainSag]->PreventUnsag = nullptr;
				}

				sagAmnt = m_sagObjects[i]->PreviousSag;
				maintainSag = i;
			}
		}

		//Determine if the player has left the segment that is currently restricting sag updates.
		//If they have, allow a sag update.
		bool preventUpdatingSag = false;
		for (SaggingSegment* segment : m_sagObjects)
		{
			if (segment->PreventUnsag)
			{
				if (!(Utils::CollisionDetection::BoxWithinBox(segment->PreventUnsag->GetPosition(), segment->PreventUnsag->GetScale(), segment->GetPosition() - Vec2{0.f, 25.f}, Vec2{ segment->GetScale().X, 50.f })))
				{
					//Player has left bounds, allow update.
					segment->PreventUnsag = nullptr;
				}
				else {
					preventUpdatingSag = true;
				}
				
			}
		}

		for (SaggingSegment* segment : m_sagObjects)
		{
			if (!preventUpdatingSag)
			{
				float sag = 0.f;

				if (segment->LeftSag && segment->RightSag)
				{
					//An adjacent segment is sagging, so it should pull this segment down.
					if ((segment->LeftSag && segment->LeftSag->IsColliding > 0) || (segment->RightSag && segment->RightSag->IsColliding > 0))
					{
						sag -= 1.f;

					}

					if (segment->IsColliding > 0 && segment->LeftSag->IsColliding < 1 && segment->RightSag->IsColliding < 1)
					{
						if (segment->LeftSag->LeftSag && segment->RightSag->RightSag)
						{
							//This is the segment that is stood on, so bring it down x2.
							sag -= 2.f;
						}
						else {
							//This is the segment that is stood on, but it is next to an edge segment -- which cannot sag -- so it should only sag x1.
							sag -= 1.f;
						}
					}
				}

				if (sag < -2.f) { sag = -2.f; } //Prevent weird stuff with multiple sag points etc.

				//This makes sure that it takes 5 update loops before a segment will ever reset to its unsagged position, ensuring that the segments do not reset
				//during the time between when the player first contacts the segments, when they are not lowered, and when the player
				//finally falls to land on the newly sagged segments.
				if (sag < 0.f)
				{
					segment->UnsagDelay = 5.f;

					if (segment->PreventUnsag)
						{
						this->m_bounceTimer++;

						if (this->m_bounceTimer > 5.f)
						{
							Components::Physics::Collision::ActiveBoxCollisionGravityObject* abcgo = dynamic_cast<Components::Physics::Collision::ActiveBoxCollisionGravityObject*>(segment->PreventUnsag);
							if (abcgo)
							{
								abcgo->GetPhysics()->AddVelocity(Vec2(0, 20.f));
							}
						}
					}
				}
				else if (segment->UnsagDelay > 0.f) {
					segment->UnsagDelay--;

					//New sag position is the last sag position, nothing should change.
					sag = segment->PreviousSag;

					this->m_bounceTimer = 0.f;
				}


				//Update to new sag position.
				segment->PreviousSag = sag;
				segment->SetPosition(Vec2{ segment->GetPosition().X, m_position.Y + (sag * m_scale.Y) });
			}
			else {
				//Update was prevented.
				segment->SetPosition(Vec2{ segment->GetPosition().X, m_position.Y + (segment->PreviousSag * m_scale.Y) });
			}

			segment->Update(window);
		}
	}

	void SaggingObject::Render(const Vec2& cameraPosition)
	{
		//Render every segment.
		for (SaggingSegment* segment : m_sagObjects)
		{
			segment->SetPosition(segment->GetPosition() + Vec2{0.f, 5.f});
			segment->Render(cameraPosition);
			segment->SetPosition(segment->GetPosition() - Vec2{ 0.f, 5.f });
		}
	}

	/// <summary>
	/// Called before any collision checks are done. This resets the IsColliding flags.
	/// </summary>
	/// <param name="self"></param>
	void SaggingObject::saggingObjectBeforeCollision(GameObject* self)
	{
		SaggingSegment* selfSag = dynamic_cast<SaggingSegment*>(self);
		if (selfSag->IsColliding > 0) { selfSag->IsColliding--; }
	}

	/// <summary>
	/// Called when a sagging object segment is collided with.
	/// </summary>
	bool SaggingObject::saggingObjectCollisionEvent(Vec2 push, GameObject* self, GameObject* other)
	{

		//--IGNORE COLLISIONS FROM BENEATH--

		//Landed on top.
		if (push.Y > 0)
		{
			SaggingSegment* selfSag = dynamic_cast<SaggingSegment*>(self);

			//Are we an edge piece, if so just allow the collision with no update to any flags.
			if (!selfSag->LeftSag || !selfSag->RightSag) { return true; }

			//An adjacent segment is colliding, so we should just allow collision.
			if (selfSag->LeftSag && selfSag->RightSag && (selfSag->LeftSag->IsColliding > 0 || selfSag->RightSag->IsColliding > 0))
			{
				return true;
			}
			else {				
				//An object was also colliding with this segment on the last frame, so the segment is now in its sagged position. We should begin preventing an unsag until the object has moved out of this segment's bounds.
				if (selfSag->IsColliding > 0)
				{
					//Ensure that an adjacent segment is not already preventing an unsag (possible if the object causing the sag is between two segments).
					if (!selfSag->LeftSag->PreventUnsag && !selfSag->RightSag->PreventUnsag)
					{
						selfSag->PreventUnsag = other;
					}
				}
				
				//We are the cause of the sag, set flag for colliding.
				selfSag->IsColliding = 4;
			}

			//This will be true if the object has just landed on the segment, which means it was in its unsagged state, and not the proper collision location, so we should prevent the collision from calculating.
			if (selfSag->PreviousSag == 0.f) { return false; }
			return true;
		}

		//Collision was not from top, ignore.
		return false;
	}
}


