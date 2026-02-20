#include "Placeable.h"
#include "../../TestGame.h"
#include "../../../Input/InputManager.h"

#include "../../../Components/Physics/Collision/Helpers/StaticBoxCollisionObject.h"

#define PLACEABLE_MAX_PLACE_DISTANCE 24.f

namespace GAME_NAME::Items
{
	double Placeable::InteractionTimer = 0.0;
	Placeable::PlaceableStateGroup* Placeable::m_placedObjects = nullptr;

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
		sprite->Render(cameraPosition, Vec2{ position.X + placeScale.X + 7.f /*IDK why 7 is here... might break stuff later...*/, position.Y + placeScale.Y}, Vec2{-placeScale.X, -placeScale.Y}, 0.f);
		delete sprite;
	}

	void Placeable::Place(Vec2 playerPosition)
	{
		Vec2 placePosition = GetPotentialPlaceLocation(playerPosition);
		Rendering::Sprite* const sprite = ITEMTYPE_GetItemTypeTexture(m_itemType);
		Vec2 placeScale = GetPlaceScale();

		//Save the state of the placed object.
		m_placedObjects->AddState(new PlacedPlaceable(placePosition, this->m_itemType, TestGame::INSTANCE->GetCurrentLevelPath()));
		
		Collision::StaticBoxCollisionObject* placedObject = new Collision::StaticBoxCollisionObject(placePosition, placeScale, sprite);
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(placedObject, false, 2, false));
	}

	void Placeable::PlaceExact(Vec2 position)
	{
		Rendering::Sprite* const sprite = ITEMTYPE_GetItemTypeTexture(m_itemType);
		Vec2 placeScale = GetPlaceScale();

		//Save the state of the placed object.
		m_placedObjects->AddState(new PlacedPlaceable(position, this->m_itemType, TestGame::INSTANCE->GetCurrentLevelPath()));

		Collision::StaticBoxCollisionObject* placedObject = new Collision::StaticBoxCollisionObject(position, placeScale, sprite);
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(placedObject, false, 2, false));
	}

	void Placeable::LoadPlaceables()
	{
		if (m_placedObjects == nullptr)
		{
			m_placedObjects = new Placeable::PlaceableStateGroup();
		}
		m_placedObjects->LoadStates();
	}

	std::unique_ptr<InventoryItem> Placeable::GetUnique()
	{
		return std::unique_ptr<Placeable>(this);
	}

	void Placeable::PlaceableStateGroup::AddState(PlacedPlaceable* placeable)
	{
		this->assignState(placeable);
	}

	void Placeable::PlaceableStateGroup::LoadStates()
	{
		//If we are loading all the placeables, we need to clear the current ones first so they get replaced.
		//Also, when the loaded states are placed, they will get added again.
		clearStates();

		auto states = this->getStates();

		for (std::string data : *states)
		{
			PlacedPlaceable p;

			p.Decode(data);
			p.Place();
		}
	}

	void Placeable::PlacedPlaceable::Decode(const SaveParam params)
	{
		std::stringstream ss(params);
		std::string line;

		int i = 0;
		while (std::getline(ss, line, ';'))
		{
			switch (i++)
			{
			case 0:
				this->Position.X = std::stof(line); break;
			case 1:
				this->Position.Y = std::stof(line); break;
			case 2:
				this->Type = static_cast<ITEM_TYPE>(std::stoi(line)); break;
			case 3:
				this->Level = line;
				std::replace(this->Level.begin(), this->Level.end(), '!', '/');
				break;
			}
		}
	}

	GAME_NAME::MiscState::SaveParam Placeable::PlacedPlaceable::Encode()
	{
		SaveParam param;

		param.append(std::to_string(this->Position.X))
			.append(";")
			.append(std::to_string(this->Position.Y))
			.append(";")
			.append(std::to_string(this->Type))
			.append(";");

		std::string savedLevel = this->Level;
		std::replace(savedLevel.begin(), savedLevel.end(), '/', '!');

		param.append(savedLevel);

		return param;
	}

	void Placeable::PlacedPlaceable::Place()
	{
		//This placeable is for another level, skip.
		if (this->Level != TestGame::INSTANCE->GetCurrentLevelPath()) { return; }

		Placeable(this->Type).PlaceExact(this->Position);
	}

}

