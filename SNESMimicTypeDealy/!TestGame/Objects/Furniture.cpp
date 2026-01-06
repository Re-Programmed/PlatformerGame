#include "Furniture.h"

#include <sstream>
#include "../Items/ItemType.h"

#include "../TestGame.h"
#include "../../Utils/CollisionDetection.h"
#include "../../Input/InputManager.h"
#include "../../Utils/Time/GameTime.h"

#include "../Level/Hub/HouseManager.h"

namespace GAME_NAME::Objects
{
	double Furniture::InteractionTimer = 0.0;

	void Furniture::RenderPreview(const Vec2& cameraPos, Items::ITEM_TYPE item, Vec2 position, bool flipped)
	{
		Sprite* sp = Items::ITEMTYPE_GetItemTypeTexture(item);
		DynamicSprite sprite(sp->GetSpriteId());
		delete sp;

		const Vec4 col = { 0.6f + ((float)InteractionTimer * 0.4f), 0.6f + ((float)InteractionTimer * 0.4f), 1.f, 0.6f + ((float)InteractionTimer * 0.4f) };

		const Vec4 Furniture_previewColors[4] = { col, col, col, col };

		sprite.UpdateTextureColor(Furniture_previewColors);

		std::string scaleData = Items::ITEMTYPE_GetItemData(item).Attributes.at(Items::FURNITURE);
		Vec2 scale{ 0 };
		scale.X = std::stof(scaleData.substr(0, scaleData.find_first_of(',')));
		scale.Y = std::stof(scaleData.substr(scaleData.find_first_of(',') + 1));

		sprite.Render(cameraPos, position + Vec2{ 0.f, scale.Y/2.f }, scale * Vec2{flipped ? 1.f : -1.f, -1.f});
	}

	void Furniture::Update(GLFWwindow* window)
	{
		LayerFlipObject::Update(window);

		m_hovered = false;

		Vec2 mousePos = InputManager::GetMouseWorldPosition(TestGame::INSTANCE->GetCamera());

		Vec2 checkPos = m_position;
		Vec2 checkScale = m_scale;
		if (m_flipped)
		{
			checkPos += Vec2{ m_scale.X, 0.f };
			checkScale = Vec2{ -m_scale.X, m_scale.Y };
		}

		if (CollisionDetection::PointWithinBoxBL(mousePos, checkPos, checkScale))
		{
			m_hovered = true;

			if (InputManager::GetMouseButton(0))
			{
				m_interactionRemovalTimer += Utils::Time::GameTime::DeltaTime::GetDeltaTime();

				if (m_interactionRemovalTimer > 1.0)
				{
					InventoryItem* item = new InventoryItem(m_item->GetType());

					if (TestGame::ThePlayer->GetInventory()->AddItem(item) == -1)
					{
						if (TestGame::ThePlayer->GetBackpack()->AddItem(item) == -1)
						{
							delete item;
							return;
						}
					}

					HouseManager::RemoveFurniture(this);
					Renderer::DestroyActiveObjectImmediate(this);
					delete this;
					return;
				}
				return;
			}
		}

		m_interactionRemovalTimer = 0.0;
	}

	void Furniture::Render(const Vec2& cameraPos)
	{
		if (m_hovered)
		{
			DynamicSprite sprite(m_sprite->GetSpriteId());

			const Vec4 col = { 1.f, 0.6f - ((float)m_interactionRemovalTimer * 0.6f), 0.6f - ((float)m_interactionRemovalTimer * 0.6f), 1.f - (float)m_interactionRemovalTimer };
			const Vec4 colors[4] = { col, col, col, col };

			sprite.UpdateTextureColor(colors);

			sprite.Render(cameraPos, m_position + m_scale, m_scale * Vec2{ -1.f }, m_rotation);
			return;
		}

		LayerFlipObject::Render(cameraPos);
	}

	//SAVEDATA: (xPos),(yPos),(itemType)

	MiscState::SaveParam Furniture::Encode()
	{
		return std::to_string(m_position.X).append(",")
			.append(std::to_string(m_position.Y)).append(",")
			.append(std::to_string(m_item->GetType())).append(",")
			.append(m_flipped ? "t" : "f");
	}

	void Furniture::Decode(const SaveParam param)
	{
		std::stringstream ss(param);

		std::string line;
		uint8_t i = 0;
		while (std::getline(ss, line, ','))
		{
			switch (i)
			{
			case 0:
				m_position.X = std::stof(line);
				break;
			case 1:
				m_position.Y = std::stof(line);
				break;
			case 2:
			{
				m_item = new Items::InventoryItem(static_cast<Items::ITEM_TYPE>(std::stoi(line)));
				//Info on the scale of the furniture is stored in the item data.
				std::string scaleInfo = Items::ITEMTYPE_GetItemData(m_item->GetType()).Attributes.at(Items::FURNITURE);

				m_scale.X = std::stof(scaleInfo.substr(0, scaleInfo.find_first_of(',')));
				m_scale.Y = std::stof(scaleInfo.substr(scaleInfo.find_first_of(',') + 1));

				SetSprite(Items::ITEMTYPE_GetItemTypeTexture(m_item->GetType()));
				break;
			}
			case 3:
				m_flipped = line == "t";
				if (m_flipped)
				{
					m_scale.X = -m_scale.X;
				}
				break;
			}

			i++;
		}

		m_collider->SetPosition(m_position);

		if (m_scale.X > 0)
		{
			m_collider->SetScale({ m_scale.X, 10.f });
		}
		else {
			m_collider->Translate(Vec2{ m_scale.X, 0.f });
			m_collider->SetScale({ -m_scale.X, 10.f });
		}
	}
}

