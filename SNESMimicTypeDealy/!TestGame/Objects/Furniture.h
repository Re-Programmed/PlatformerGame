#pragma once

#include "../../Objects/GameObject.h"
#include "../Items/InventoryItem.h"
#include "./LayerFlipObject.h"

#include "../../Components/Physics/Collision/Helpers/StaticBoxCollisionObject.h"
#include "../Items/Inventories/InventoryContainer.h"
#include "../Items/Inventories/InventoryContainerRenderer.h"

#include "../Cutscenes/Character/CharacterNodeManager.h"
#include "../Cutscenes/Character/AnimatingCharacter.h"

namespace GAME_NAME::Objects
{
	using namespace GAME_NAME::Components::Physics::Collision;

	constexpr float FURNITURE_PLACE_OFFSET = 12.f;

	class Furniture
		: public LayerFlipObject, public MiscState, public Cutscenes::ICharacterNode
	{
	public:
		struct FurnitureItemInfo
		{
			Vec2 Scale = Vec2{ 0, 0 };
			uint8_t InventorySize = 0;
			bool IsOutdoor = false;
		};

		inline FurnitureItemInfo GetInfo() { return Furniture::GetInfo(Items::ITEMTYPE_GetItemData(m_item->GetType()).Attributes.at(Items::FURNITURE)); }

		static FurnitureItemInfo GetInfo(std::string furnitureData);

		const Vec2& GetPosition()
		{
			return m_position;
		}

		///Used by an animating character.
		bool Use(Cutscenes::AnimatingCharacter * character);

		enum class FurnitureInteractions
		{
			None,
			Sit,
			Inventory,
			Workbench
		};
		
		static const std::unordered_map<Items::ITEM_TYPE, FurnitureInteractions> FurnitureAbilities;

		static double InteractionTimer;

		static void RenderPreview(const Vec2& cameraPos, Items::ITEM_TYPE item, Vec2 position, bool flipped);

		Furniture()
			: LayerFlipObject(Vec2{ 0 }, Vec2{ 0 }, nullptr, -3.f), m_item(nullptr), m_flipped(false), m_collider(new StaticBoxCollisionObject(Vec2{0}, Vec2{0}, nullptr))
		{
			Renderer::LoadObject(m_collider, 0);
			
		}

		~Furniture()
		{
			delete m_item;
			Renderer::DestroyObject(m_collider);

			if (m_inventory != nullptr)
			{
				Renderer::DestroyObject(m_inventory);
			}

			if (m_isOutdoor)
			{
				Cutscenes::CharacterNodeManager::RemoveNode(this);
			}

			//LayerFlipObject::~LayerFlipObject();
		}

		Furniture(Vec2 position, bool flipped, Items::InventoryItem* placedItem)
			: LayerFlipObject(position, Vec2{ 0 }, nullptr, -3.f), m_item(placedItem), m_flipped(flipped), m_collider(new StaticBoxCollisionObject(position, Vec2{ 0 }, nullptr))
		{
			Renderer::LoadObject(m_collider, 0);

			FurnitureItemInfo info = GetInfo();

			//Sprite and scale data are stored based on the item that this furniture is.
			
			m_scale.X = (m_flipped ? -1.f : 1.f) * info.Scale.X;
			m_scale.Y = info.Scale.Y;

			if (m_scale.X > 0)
			{
				m_collider->Translate(Vec2{ -m_scale.X, 0.f });
				m_collider->SetScale({ m_scale.X, 10.f });
			}
			else {
				m_collider->Translate(Vec2{ -m_scale.X, 0.f });
				m_collider->SetScale({ -m_scale.X, 10.f });
			}

			SetSprite(Items::ITEMTYPE_GetItemTypeTexture(m_item->GetType()));

			if (FurnitureAbilities.at(m_item->GetType()) == FurnitureInteractions::Inventory)
			{
				uint8_t& invSize = info.InventorySize;

				m_inventory = new Items::Inventories::InventoryContainer(ITEMTYPE_GetItemTypeName(m_item->GetType()), invSize, m_position - Vec2{ m_scale.X < 0.f ? 4.f - m_scale.X : 4.f, 4.f }, (m_scale.X < 0.f ? Vec2{ -m_scale.X, m_scale.Y } : m_scale) + Vec2{ 8.f, 8.f }, nullptr, 0, 0.f, false);
				Renderer::InstantiateObject(Renderer::InstantiateGameObject(m_inventory, false, 1, false));
			}

			m_isOutdoor = info.IsOutdoor;

			//If outdoor, characters will be able to interact with this.
			if (m_isOutdoor)
			{
				Cutscenes::CharacterNodeManager::RegisterNode(this);
			}
		}

		inline bool IsOutdoor()
		{
			return m_isOutdoor;
		}

		void Update(GLFWwindow* window) override;

		void Render(const Vec2& cameraPos) override;

		SaveParam Encode() override;
		void Decode(const SaveParam param) override;

	protected:
		void use();
		void useSit();

		Items::InventoryItem* m_item SERIALIZED;
		bool m_flipped SERIALIZED;

	private:
		Items::Inventories::InventoryContainer* m_inventory = nullptr;

		bool m_inUse = false;
		Cutscenes::AnimatingCharacter* m_inUseByCharacter = nullptr;

		double m_interactionRemovalTimer = 0.0;

		bool m_isOutdoor = false;

		bool m_hovered = false;
		StaticBoxCollisionObject* m_collider;
	};
}