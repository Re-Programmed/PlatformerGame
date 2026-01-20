#pragma once
#include "../../../Objects/GUI/StaticGUIElement.h"
#include "../../../Objects/GUI/GUIButton.h"
#include "../../Objects/Player/Player.h"
#include "../../../Input/InputManager.h"
#include "../../TestGame.h"
#include "../../InputDisplay/DisplayIconManager.h"
#include "../../../Objects/Helpers/Interactable.h"

#include "../../../Objects/GUI/Text/TextRenderer.h"

#include "../../../Objects/StateSaver.h"

namespace GAME_NAME::Level
{
#define DEPART_MANAGER_LEVEL_COUNT 1

	class DepartManager
	{
	public:
		enum class LevelID
		{
			GreenRegion
		};

		static void Load();

		static void Update();

		static void OpenDepartMenu();
		/// <summary>
		/// Attempts to close the depart menu. Returns false if it was not open.
		/// </summary>
		/// <returns></returns>
		static bool CloseDepartMenu();

	private:

		static LevelID m_currentDisplayedLevel;
		static uint8_t m_currentSelectedArea;

		static void LoadLevelDisplay(LevelID level);

		class DepartMenuInteractable
			: public Interactable
		{
		public:
			DepartMenuInteractable()
				: Interactable(keyRef::PLAYER_INTERACT, InputManager::KEY_STATE_NONE, Vec2{ 462.f, 29.f }, Vec2{ 50.f, 14.f }, nullptr, 0.f, true)
			{

			}

		protected:
			void onInteract(std::shared_ptr<Objects::Player::Player> player, InputManager::KEY_STATE keyState) override
			{
				Input::DisplayIconManager::ShowKeyInputDisplay(Input::DisplayIconManager::INPUT_DISPLAY_KEY_E, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), 0);

				if (keyState & InputManager::KEY_STATE_PRESSED)
				{
					if (!CloseDepartMenu())
					{
						OpenDepartMenu();
					}
				}
			}
		};

		class LevelSelectionData
			: public MiscState
		{
		public:
			const LevelID LevelID;
			const int LevelDisplaySprite;
			const std::string Name1;
			const std::string Name2;
			const std::string Name3;
			const std::string Area1;
			const std::string Area2;
			const std::string Area3;

			LevelSelectionData(DepartManager::LevelID levelID, int levelDisplaySprite, std::string name1, std::string name2, std::string name3, std::string area1, std::string area2, std::string area3, bool unlocked = false)
				: MiscState(), LevelID(levelID), LevelDisplaySprite(levelDisplaySprite), Name1(name1), Name2(name2), Name3(name3), Area1(area1), Area2(area2), Area3(area3), m_unlocked(unlocked)
			{
				
			}

			inline bool IsUnlocked()
			{
				return m_unlocked;
			}

			void Unlock();

			void Decode(const SaveParam params) override;
			SaveParam Encode() override;

		private:
			bool m_unlocked SERIALIZED;
		};

		class LevelSelector
			: public MiscStateGroup
		{
		public:
			/// <summary>
			/// Create a level selection list with the following levels.
			/// </summary>
			/// <param name="data">[std::array<LevelSelectionData, DEPART_MANAGER_LEVEL_COUNT>] - Data.</param>
			LevelSelector(std::array<LevelSelectionData, DEPART_MANAGER_LEVEL_COUNT> data)
				: MiscStateGroup("ls"), m_levelSelections(data)
			{
				
			}

			std::array<LevelSelectionData, DEPART_MANAGER_LEVEL_COUNT>& Get()
			{
				return m_levelSelections;
			}

			/// <summary>
			/// Unlocks and saves the specified level.
			/// </summary>
			/// <param name="level">[LevelID] - The level to unlock.</param>
			/// <returns></returns>
			const void Unlock(LevelID level)
			{
				clearStates();

				for (LevelSelectionData data : m_levelSelections)
				{
					if (data.LevelID == level)
					{
						data.Unlock();
					}

					assignState(&data);
				}
			}

			inline void AllowSave()
			{
				//Will not allow duplicates.
				Objects::StateSaver::RegisterToBeSaved(this);
			}

			inline void Load()
			{
				auto states = *(getStates().get());
				for (int i = 0; i < DEPART_MANAGER_LEVEL_COUNT; i++)
				{
					if (states.size() > i)
					{
						m_levelSelections[i].Decode(states[i]);
					}

					assignState(&m_levelSelections[i]);
				}
			}

		private:
			/// <summary>
			/// The levels that can be chosen.
			/// </summary>
			std::array<LevelSelectionData, DEPART_MANAGER_LEVEL_COUNT> m_levelSelections;
		};

		/// <summary>
		/// All elements in the departure menu.
		/// </summary>
		struct DepartureSelectMenu
		{
			GUI::StaticGUIElement *Backing = nullptr, *DarkenedBack = nullptr;
			DepartMenuInteractable* Interactable = nullptr;
			GUI::GUIButton *LevelAdvanceButton = nullptr, *LevelBackButton = nullptr;

			GUI::StaticGUIElement* LevelDisplay = nullptr;
			GUI::Text::TextRenderer::ExpectedRenderedWord Area1Title; GUI::GUIButton* Area1Button = nullptr;
			GUI::Text::TextRenderer::ExpectedRenderedWord Area2Title; GUI::GUIButton* Area2Button = nullptr;
			GUI::Text::TextRenderer::ExpectedRenderedWord Area3Title; GUI::GUIButton* Area3Button = nullptr;

			GUI::GUIButton* Bell = nullptr;
		};

		static LevelSelector m_levelSelections;

		static DepartureSelectMenu m_menu;

		static void buttonCallback(int buttonID);
	};
}