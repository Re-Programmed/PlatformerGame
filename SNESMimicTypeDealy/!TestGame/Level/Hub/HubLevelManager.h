#pragma once
#include "../LevelSystem.h"

#include "../../../Objects/GameObject.h"

#include "../../Objects/Player/Player.h"
#include "../../Objects/GUI/GUIItemSelectionBox.h"

#include <array>

namespace GAME_NAME::Level
{
	class HubLevelManager
		: public LevelSystem
	{
	public:
		HubLevelManager();

		void Update(GLFWwindow* window) override;

		inline void SetUnlockHouseLevel(const int& level)
		{
			m_currentHubCharacteristics.PlayerHouseLevel.second = level;
			m_currentHubCharacteristics.Save();
			updateHouseUnlock();
		}

		inline int GetUnlockHouseLevel()
		{
			return m_currentHubCharacteristics.PlayerHouseLevel.second;
		}

		static bool OpenShopGUI();

		static bool CloseShopGUI();

	private:
		static void generateShop();

		static void saveShop();
		static std::pair<std::vector<GUIItemSelectionBox::ItemSelection>, std::vector<GUIItemSelectionBox::ItemSelection>> loadShop();

		/// <summary>
		/// Removes an item from a selection box based on some given condition.
		/// </summary>
		/// <param name="box"></param>
		/// <param name="condition"></param>
		/// <returns></returns>
		static bool removeItemFromSelectionBox(GUIItemSelectionBox*& box, std::function<bool(GUIItemSelectionBox::ItemSelection)> condition, std::function<void(GAME_NAME::Items::ITEM_TYPE, int)>* selectCallback);

		static void performPurchase(ITEM_TYPE crumbType, int count);

		static bool m_shopGUIOpen;

		/// <summary>
		/// Updates what objects are enabled in the hub to display the house at the current unlock level.
		/// </summary>
		void updateHouseUnlock();

		/// <summary>
		/// Called on entering the shop area to initilize the teller and stuff.
		/// </summary>
		void loadShopArea();


		/// <summary>
		/// Save data for the shop.
		/// </summary>
		class ShopData
			: public MiscStateGroup
		{
		public:
			ShopData()
				: MiscStateGroup("shop")
			{

			}

			std::shared_ptr<std::vector<std::string>> GetItems()
			{
				return getStates();
			}

			void ClearItems()
			{
				this->clearStates();
			}

			void AddItem(GUIItemSelectionBox::ItemSelection item)
			{
				this->assignState(new ShopItem(item));
			}

			class ShopItem
				: public MiscState
			{
			public:
				ShopItem(std::string data)
				{
					this->Decode(data);
				}

				ShopItem(GUIItemSelectionBox::ItemSelection selection)
					: m_selection(selection)
				{

				}

				inline GUIItemSelectionBox::ItemSelection GetSelection()
				{
					return m_selection;
				}

				void Decode(const SaveParam params)
				{
					m_selection.Type = static_cast<ITEM_TYPE>(std::stoi(params.substr(0, params.find_first_of(','))));
					m_selection.Count = std::stoi(params.substr(params.find_first_of(',') + 1));
				}

				SaveParam Encode()
				{
					SaveParam param;
					param.append(std::to_string(m_selection.Type))
						.append(",")
						.append(std::to_string(m_selection.Count));

					return param;
				}

			private:
				GUIItemSelectionBox::ItemSelection m_selection;
			};
		};

		/// <summary>
		/// Stores the data for the currently open ShopGUI.
		/// </summary>
		struct ShopGUI
		{
			GUIItemSelectionBox* ItemIn;
			GUIItemSelectionBox* ItemOut;
			ShopData* Data = new ShopData();
		};

		/// <summary>
		/// The currently open shop.
		/// </summary>
		static ShopGUI m_currentShop;

		struct HubCharactersitics
		{
			std::pair<std::string, int> PlayerHouseLevel = std::make_pair("PlayerHouseLevel", 1);

			void Load();
			void Save();
		};

		HubCharactersitics m_currentHubCharacteristics;

	};
}