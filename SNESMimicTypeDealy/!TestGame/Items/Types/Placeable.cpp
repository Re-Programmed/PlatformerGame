#include "Placeable.h"
#include "../../TestGame.h"
#include "../../../Input/InputManager.h"

#include "../../../Components/Physics/Collision/Helpers/StaticBoxCollisionObject.h"

#define PLACEABLE_MAX_PLACE_DISTANCE 24.f

namespace GAME_NAME::Items
{
	double Placeable::InteractionTimer = 0.0;

	Placeable::Placeable(ITEM_TYPE type)
		: InventoryItem(type)
	{

	}

	Vec2 Placeable::GetPlaceScale()
	{
		std::string data = ITEM_DATA[m_itemType].Attributes.at(TOOL_ACTION::PLACEABLE);
		return Vec2{ std::stof(data.substr(0, data.find_first_of(','))), std::stof(data.substr(data.find_first_of(',') + 1)) };
	}

	Vec2 Placeable::GetPotentialPlaceLocation(Vec2& playerPosition)
	{
		playerPosition -= Vec2{11.55f, 0.f}; //Adjust for the weird offset player texture... weirdo magic numbers yay!
		Vec2 placeScale = this->GetPlaceScale();
		Vec2 potentialPosition = InputManager::GetMouseWorldPosition(TestGame::INSTANCE->GetCamera()) - placeScale / 2.f;
		if (Vec2::Distance(playerPosition, potentialPosition) > PLACEABLE_MAX_PLACE_DISTANCE)
		{
			Vec2 offset = potentialPosition - playerPosition;
			float offsetLen = offset.Length();

			potentialPosition = playerPosition + offset * (PLACEABLE_MAX_PLACE_DISTANCE / offsetLen);
		}

		return potentialPosition;
	}

	void Placeable::RenderPreview(const Vec2& cameraPosition, const Vec2& position)
	{
		Sprite* s = ITEMTYPE_GetItemTypeTexture(this->m_itemType);
		DynamicSprite* sprite = new DynamicSprite(s->GetSpriteId());

		float transparency = InteractionTimer > 0.0 ? static_cast<float>(InteractionTimer) : 0.33f;

		delete s;
		Vec4 cols[4] = {
			Vec4{ 1.f, 1.f, 1.f, transparency },
			Vec4{ 1.f, 1.f, 1.f, transparency },
			Vec4{ 1.f, 1.f, 1.f, transparency },
			Vec4{ 1.f, 1.f, 1.f, transparency }
		};
		sprite->UpdateTextureColor(cols);

		Vec2 placeScale = this->GetPlaceScale();
		sprite->Render(cameraPosition, Vec2{ position.X + placeScale.X + 7.f /*IDK why 7 is here... might break stuff later...*/, position.Y}, Vec2{-placeScale.X, placeScale.Y}, 0.f);
		delete sprite;
	}

	void Placeable::Place(Vec2 playerPosition)
	{
		Vec2 placePosition = GetPotentialPlaceLocation(playerPosition);

		Collision::StaticBoxCollisionObject* placedObject = new Collision::StaticBoxCollisionObject(placePosition, GetPlaceScale(), ITEMTYPE_GetItemTypeTexture(m_itemType));
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(placedObject, false, 2, false));
	}

	GAME_NAME::MiscState::SaveParam Placeable::Encode()
	{
		return InventoryItem::Encode();
	}

	void Placeable::Decode(const SaveParam params)
	{
		InventoryItem::Decode(params);
	}
}

