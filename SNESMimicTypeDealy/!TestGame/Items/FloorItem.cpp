#include "FloorItem.h"
#include "../../Input/InputManager.h"
#include "../../Utils/Time/GameTime.h"
#include "../TestGame.h"

#define FLOOR_ITEM_GLOW_SPRITE SpriteBase(131) //The sprite rendered behind items.
#define ITEM_BOB_ANIMATION_SPEED 1.f

namespace GAME_NAME
{
	namespace Items
	{
		std::shared_ptr<Rendering::DynamicSprite> FloorItem::m_glowSprite = nullptr;

		FloorItem::~FloorItem()
		{
			ActiveRotationalBoxCollisionGravityObject::~ActiveRotationalBoxCollisionGravityObject();
			delete m_inventoryItem;
		}

		FloorItem::FloorItem(Vec2 position, InventoryItem* item, float pickupDelay)
			: ActiveRotationalBoxCollisionGravityObject(position, Vec2(DF_FLOOR_ITEM_SCALE), ITEMTYPE_GetItemTypeTexture(item->GetType()), RotationalCollider_Settings(
				53.0f,		//Bounce
				3.25f,		//Rotation Effect
				7.50f		//Gravity Rotation Effect
			)), m_inventoryItem(item), m_lifetime(0), m_pickupDelay(pickupDelay)
		{
			m_physics->SetGravityStrength(m_physics->GetGravityStrength() * 7);

			if (!m_glowSprite)
			{
				auto s = Renderer::GetSprite(FLOOR_ITEM_GLOW_SPRITE);

				m_glowSprite = std::make_shared<DynamicSprite>(s->GetSpriteId());

				Vec4 transparentColor[4] = {
					{ 1.f, 1.f, 1.f, 0.3f },
					{ 1.f, 1.f, 1.f, 0.3f },
					{ 1.f, 1.f, 1.f, 0.3f },
					{ 1.f, 1.f, 1.f, 0.3f }
				};

				m_glowSprite->UpdateTextureColor(transparentColor);

				delete s;
			}
		}
		
		void FloorItem::Update(GLFWwindow* window)
		{
			//Check if the item has been picked up.
			if (m_pickedUp) { return; }

			//Track the lifetime of the item for pickup delay.
			m_lifetime += (float)Utils::Time::GameTime::GetScaledDeltaTime();

			if (m_lifetime >= m_pickupDelay)
			{
				if (Vec2::Distance(TestGame::ThePlayer->GetPosition(), m_position) < m_scale.X/2.f)
				{
					pickup();
				}
			}

			ActiveRotationalBoxCollisionGravityObject::Update(window);
		}

		void FloorItem::pickup()
		{
			//Attempt to add the item to the player's inventory.
			if (TestGame::ThePlayer->GetInventory()->AddItem(m_inventoryItem) >= 0)
			{
				//Sucessful, destroy the item.
				m_pickedUp = true;
				Renderer::DestroyActiveObjectImmediate(this);
				//delete this;
			}
		}
		
		//Global stored oscilation values for all items to avoid recalculation.
		float osc, osc2, osc3;
		std::uint8_t pass = 0;

		void FloorItem::Render(const Vec2& cameraPosition)
		{
			//Only update these oscilating values once for all items to avoid recalculation of a variable that only needs to be determined one time.
			if (m_renderPass == pass)
			{
				osc = (float)std::sin(glfwGetTime() * ITEM_BOB_ANIMATION_SPEED);
				osc2 = (float)std::sin(1.f + glfwGetTime() * ITEM_BOB_ANIMATION_SPEED * 1.5f);
				osc3 = (float)std::sin(0.5f + glfwGetTime() * ITEM_BOB_ANIMATION_SPEED * 1.25f);

				if (m_renderPass % 2 == 1)
				{
					pass--;
					m_renderPass--;
				}
				else {
					pass++;
					m_renderPass++;
				}
			}

			const Vec2 pos = m_position + Vec2{ 0.f, 1.5f + osc3 * 0.66f };
			const Vec2 sca = m_scale + Vec2{ osc2/1.5f };

			m_glowSprite->Render(cameraPosition, pos, sca); //Render glowing sprite.
			m_sprite->Render(cameraPosition, pos, sca, osc * 4.f); //Render item.
		}
	}
}