#pragma once
#include <vector>
#include "../../Objects/Furniture.h"

#include "../../../Objects/MiscStateGroup.h"
#include "../../Objects/GUI/GUIItemSelectionBox.h"

#include "../../../Objects/Helpers/Interactable.h"
#include "../../Objects/Environment/Buildings/InnerUpperWall.h"

#define HOUSE_MANAGER_FURNITURE_TRADE_SAVE_LOCATION "house_trades"

namespace GAME_NAME::Level
{
	class HouseManager
		: public MiscStateGroup
	{
	public:
		HouseManager()
			: MiscStateGroup("house")
		{

		}

		static void LoadHouse();
		static void CloseHouse();

		/// <summary>
		/// Stashes all furniture in the player's inventory into the saved furniture inventory.
		/// </summary>
		static void StorePlayerInventory();

		static void OpenFurnitureInventory();
		static bool CloseFurnitureInventory();

		static bool PlaceFurniture(Furniture* const furniture);
		static bool RemoveFurniture(Furniture* const furniture);

		static bool CheckValidPlaceLocation(const Vec2 pos, const Vec2 scale);
	private:
		static void populateStore();

		class FurnitureTrade
			: public MiscState
		{
		public:
			int Cost = 0;
			Items::ITEM_TYPE Product = Items::ITEM_TYPE::NULL_ITEM;

			SaveParam Encode() override
			{
				return SaveParam(std::to_string(Product)).append(",").append(std::to_string(Cost));
			}
			void Decode(const SaveParam param) override
			{
				Product = static_cast<Items::ITEM_TYPE>(std::stoi(param.substr(0, param.find_first_of(','))));
				Cost = std::stoi(param.substr(param.find_first_of(',') + 1));
			}
		};

		static std::array<FurnitureTrade, 7> m_furnitureTrades;

		template<typename T>
		static void storeInventory(T* inventory);

		static void furnitureInventoryCallback(Items::ITEM_TYPE type, int count);
			
		static void furnitureTradeCallback(Items::ITEM_TYPE type, int count);

		typedef struct FurnitureMenu
		{
			GUI::GUIItemSelectionBox* SelectionMenu = nullptr;
			GUI::GUIItemSelectionBox* PurchaseMenuMoney = nullptr;
			GUI::GUIItemSelectionBox* PurchaseMenuItem = nullptr;
			GUI::StaticGUIElement* Backing = nullptr;
		};

		static FurnitureMenu m_furnitureMenu;

		class HouseTradeSaveManager
			: public MiscStateGroup
		{
		public:
			HouseTradeSaveManager()
				: MiscStateGroup(HOUSE_MANAGER_FURNITURE_TRADE_SAVE_LOCATION)
			{

			}

			void LoadFurnitureTrades()
			{
				int added = 0;

				auto states = getStates();
				for(std::string state : (*states.get()))
				{
					if (added >= m_furnitureTrades.size()) { return; }

					m_furnitureTrades[added++].Decode(state);
				}
			}

			void UpdateSavedFurnitureTrades()
			{
				clearStates();

				for (FurnitureTrade& trade : m_furnitureTrades)
				{
					assignState(&trade);
				}
			}
		};

		static HouseTradeSaveManager* m_houseTradeSaveManager;

		class HouseInventorySaveManager
			: public MiscStateGroup
		{
		public:
			HouseInventorySaveManager()
				: MiscStateGroup("house_inv")
			{

			}

			class SavedItemType
				: public MiscState
			{
			public:
				SavedItemType(Items::ITEM_TYPE type)
					: Type(type)
				{

				}

				Items::ITEM_TYPE Type;

				SaveParam Encode() override;
				void Decode(const SaveParam param);
			};

			inline void AddItem(Items::ITEM_TYPE type)
			{
				m_contents.push_back(new SavedItemType(type));
				assignState(m_contents[m_contents.size() - 1]);
			}

			inline void AddItem(SavedItemType* type)
			{
				m_contents.push_back(type);
				assignState(m_contents[m_contents.size() - 1]);
			}

			/// <summary>
			/// Clears and reloads all save states.
			/// Necessary when removing items.
			/// </summary>
			inline void RefreshStates()
			{
				clearStates();
				for (SavedItemType* item : m_contents)
				{
					assignState(item);
				}
			}

			void Load()
			{
				auto states = getStates();
				for (std::string state : *(states.get()))
				{
					SavedItemType* item = new SavedItemType(Items::ITEM_TYPE::NULL_ITEM);
					item->Decode(state);

					AddItem(item);
				}
			}

			inline std::vector<Items::ITEM_TYPE> GetItems()
			{
				std::vector<Items::ITEM_TYPE> ret;
				ret.reserve(5);

				for (int i = 0; i < m_contents.size(); i++)
				{
					ret.emplace_back(m_contents[i]->Type);
				}

				return ret;
			}

			inline bool RemoveItem(Items::ITEM_TYPE type)
			{
				for (int i = 0; i < m_contents.size(); i++)
				{
					if (m_contents[i]->Type == type)
					{
						SavedItemType* sit = m_contents[i];
						m_contents.erase(m_contents.begin() + i);
						delete sit;

						RefreshStates();
						return true;
					}
				}

				return false;
			}

		private:
			std::vector<SavedItemType*> m_contents;
		};
		/// <summary>
		/// Active while the house is open.
		/// </summary>
		static HouseManager* INSTANCE;
		static HouseInventorySaveManager* INVENTORY_INSTANCE;

		static std::vector<Furniture*> m_placedFurniture;

		/// <summary>
		/// Loads the selection box for items in the GUI.
		/// </summary>
		static void loadFurnitureSelectionScroll();

		class FurnitureStoreInteractable
			: public Interactable
		{
		public:
			FurnitureStoreInteractable(Vec2 position, Vec2 scale, Rendering::Sprite* sprite)
				: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, scale.Y + 8.f, position, scale, sprite)
			{

			}

			void Render(const Vec2& cameraPos)
			{
				const float playerY = TestGame::ThePlayer->GetPosition().Y;

				if (playerY < m_position.Y)
				{
					GameObject::Render(cameraPos);
					return;
				}

				if (playerY > m_position.Y && playerY < m_position.Y + m_scale.Y)
				{
					float prop = 1.f - ((playerY - m_position.Y) / m_scale.Y);

					DynamicSprite transparentSprite(m_sprite->GetSpriteId());

					const Vec4 col{ 1.f, 1.f, 1.f, prop };
					const Vec4 colors[4] = { col, col, col, col };

					transparentSprite.UpdateTextureColor(colors);
					transparentSprite.Render(cameraPos, m_position + m_scale, Vec2{ -m_scale.X, -m_scale.Y }, m_rotation);
					return;
				}

				//Don't render since the player is above the wall.
			}

		protected:
			void onInteract(std::shared_ptr<GAME_NAME::Objects::Player::Player> player, InputManager::KEY_STATE state) override;
		};

		static FurnitureStoreInteractable* m_storeInteractable;
	};

}