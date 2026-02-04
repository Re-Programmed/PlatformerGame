#pragma once
#include "../../Objects/GUI/StaticGUIElement.h"
#include "../../Objects/GUI/GUIButton.h"
#include "../../Objects/GUI/Text/TextRenderer.h"

#include "./GlobalLevelData.h"

#include "../Objects/GUI/AnimatingGUIElement.h"

namespace GAME_NAME::Level
{
	class LevelCompleteMenu
	{
	public:
		static void DisplayMenu(std::string levelDestination);

		static inline bool IsDisplayed() { return m_levelCompleteMenuObjects.Backing != nullptr; }

		static void Update();

	private:
		static uint8_t m_animationStage;

		static const std::unordered_map<GlobalLevelData::Score, int> m_scoreSprites;

		static std::string m_levelDestination;

		struct LevelCompleteMenuObjects
		{
			GUI::StaticGUIElement* Backing = nullptr;
			GUI::GUIButton* ContinueButton = nullptr;
			GUI::Text::TextRenderer::RenderedWord ContinueWord;
			GUI::Text::TextRenderer::ExpectedRenderedWord TimeWord;
			GUI::AnimatingGUIElement::AnimatingNumber ElapsedMin, ElapsedSec;

			GUI::Text::TextRenderer::ExpectedRenderedWord PointsWord;
			GUI::AnimatingGUIElement::AnimatingNumber PointsAmount;

			GUI::Text::TextRenderer::ExpectedRenderedWord ToastWord;
			std::array<GUI::AnimatingGUIElement*, 3> CollectedToast = { nullptr, nullptr, nullptr };

			GUI::StaticGUIElement* FinalScoreDelim = nullptr;
			GUI::Text::TextRenderer::ExpectedRenderedWord ScoreText;
			GUI::AnimatingGUIElement* ScoreResult = nullptr;
		};

		static LevelCompleteMenuObjects m_levelCompleteMenuObjects;

		static void closeMenu();
	};
}