#include "LevelCompleteMenu.h"
#include "../../Objects/GUI/GUIManager.h"

#include "../TestGame.h"

#include "../../Input/InputManager.h"

#include "../../Utils/Time/GameTime.h"

#include "../Objects/Collectables/CollectableTracker.h"

#define LEVEL_COMPLETE_MENU_BG_SPRITE SpriteBase(268)
#define LEVEL_COMPLETE_MENU_BUTTON_SPRITE SpriteBase(269)
#define LEVEL_COMPLETE_MENU_DELIM_LINE_SPRITE SpriteBase(271)

#define LEVEL_COMPLETE_MENU_TOAST_GOTTEN SpriteBase(132)
#define LEVEL_COMPLETE_MENU_TOAST_MISSED SpriteBase(272)

namespace GAME_NAME::Level
{
using namespace GUI;

	uint8_t LevelCompleteMenu::m_animationStage = 0;

	LevelCompleteMenu::LevelCompleteMenuObjects LevelCompleteMenu::m_levelCompleteMenuObjects;
	std::string LevelCompleteMenu::m_levelDestination = "";

	const std::unordered_map<GlobalLevelData::Score, int> LevelCompleteMenu::m_scoreSprites = {
		{ GlobalLevelData::Score::AP, SpriteBase(274) },
		{ GlobalLevelData::Score::A, SpriteBase(273) },
		{ GlobalLevelData::Score::B, SpriteBase(275) },
		{ GlobalLevelData::Score::C, SpriteBase(276) },
		{ GlobalLevelData::Score::D, SpriteBase(277) },
		{ GlobalLevelData::Score::F, SpriteBase(278) },
		{ GlobalLevelData::Score::FF, SpriteBase(279) }
	};

	double LevelCompleteMenu_MouseClickDelay = 0.0;

	void LevelCompleteMenu::Update()
	{
		if (!IsDisplayed()) { return; }

		if (LevelCompleteMenu_MouseClickDelay > 0.0)
		{
			LevelCompleteMenu_MouseClickDelay -= Utils::Time::GameTime::DeltaTime::GetDeltaTime();
		}

		if (InputManager::GetMouseButton(0) && LevelCompleteMenu_MouseClickDelay <= 0.0)
		{
			LevelCompleteMenu_MouseClickDelay = 0.66;
			m_animationStage++;

			switch (m_animationStage)
			{
			case 1:
				for (AnimatingGUIElement* digit : m_levelCompleteMenuObjects.ElapsedMin) { digit->StartAnimation(0.0175); }
				for (AnimatingGUIElement* digit : m_levelCompleteMenuObjects.ElapsedSec) { digit->StartAnimation(0.0175); }
				break;
			case 2:
				for (AnimatingGUIElement* digit : m_levelCompleteMenuObjects.PointsAmount) { digit->StartAnimation(0.0175); }
				break;
			case 3:
				m_levelCompleteMenuObjects.CollectedToast[0]->StartAnimation(0.0175);
				m_levelCompleteMenuObjects.CollectedToast[1]->StartAnimation(0.015);
				m_levelCompleteMenuObjects.CollectedToast[2]->StartAnimation(0.0125);
				break;
			case 4:
				m_levelCompleteMenuObjects.ScoreResult->StartAnimation(0.025);
				break;
			case 5:
				//Make button and text to continue visible.
				Renderer::LoadGUIElement(m_levelCompleteMenuObjects.ContinueButton);
				GUIManager::RegisterButton(m_levelCompleteMenuObjects.ContinueButton);

				m_levelCompleteMenuObjects.ContinueWord = Text::TextRenderer::RenderWord("DONE", m_levelCompleteMenuObjects.ContinueButton->GetPosition() + Vec2{ 14.f, 4.f }, 12.f, -0.5f, 1);
				break;
			}
		}
	}

	void LevelCompleteMenu::DisplayMenu(std::string levelDestination)
	{
		m_levelDestination = levelDestination;

		m_levelCompleteMenuObjects.Backing = new StaticGUIElement(Vec2{ TargetResolutionX, 0.f }, Vec2{ -TargetResolutionX, TargetResolutionY }, Renderer::GetSprite(LEVEL_COMPLETE_MENU_BG_SPRITE));
		Renderer::LoadGUIElement(m_levelCompleteMenuObjects.Backing);

		m_levelCompleteMenuObjects.ContinueButton = new GUIButton(Vec2{ TargetResolutionX - 110.f, 33.5f }, Vec2{ 50.f, 18.f }, Renderer::GetSprite(LEVEL_COMPLETE_MENU_BUTTON_SPRITE), new std::function<void(int)>([](int buttonID) { closeMenu(); }));

#pragma region Time
		int elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - GlobalLevelData::GetLevelStartTime().count();
		int elapsedMinutes = elapsedSeconds / 60;
		elapsedSeconds = elapsedSeconds % 60;

		m_levelCompleteMenuObjects.TimeWord = Text::TextRenderer::RenderWordCaseSensitive("Time:", Vec2{ 80.f, TargetResolutionY - 40.f }, 10.f, -1.75f, 1);

		Text::TextRenderer::RenderedDigit elapsedMinNumber = Text::TextRenderer::RenderNumber(elapsedMinutes, Vec2{ 120.f, TargetResolutionY - 40.f }, 0.43f, -1.2f, 2);
		m_levelCompleteMenuObjects.ElapsedMin = AnimatingGUIElement::CreateAnimatingNumber(elapsedMinNumber);
		Text::TextRenderer::RenderedDigit elapsedSecNumber = Text::TextRenderer::RenderNumber(elapsedSeconds, Vec2{ 140.f, TargetResolutionY - 40.f }, 0.43f, -1.2f, 2);
		m_levelCompleteMenuObjects.ElapsedSec = AnimatingGUIElement::CreateAnimatingNumber(elapsedSecNumber);

		//Hide the text so it can animate in.
		for (AnimatingGUIElement* element : m_levelCompleteMenuObjects.ElapsedMin) { element->SetOpacity(0.f); }
		for (AnimatingGUIElement* element : m_levelCompleteMenuObjects.ElapsedSec) { element->SetOpacity(0.f); }

		auto colon = Text::TextRenderer::RenderWordCaseSensitive(":", Vec2{ 136.5f, TargetResolutionY - 40.f }, 10.f, 0.f, 1);
		m_levelCompleteMenuObjects.TimeWord.push_back(colon[0]);
#pragma endregion

#pragma region Points
		m_levelCompleteMenuObjects.PointsWord = Text::TextRenderer::RenderWordCaseSensitive("Points:", Vec2{ 80.f, TargetResolutionY - 58.f }, 10.f, -1.75f, 1);

		auto pointNumber = Text::TextRenderer::RenderNumber(GlobalLevelData::GetPoints(), Vec2{ 130.f, TargetResolutionY - 58.f }, 0.43f, -1.2f, 1);
		m_levelCompleteMenuObjects.PointsAmount = AnimatingGUIElement::CreateAnimatingNumber(pointNumber);

		//Hide and move each number down.
		for (AnimatingGUIElement* element : m_levelCompleteMenuObjects.PointsAmount) { element->SetOpacity(0.f); element->SetPosition(element->GetPosition() - Vec2{ 0.f, element->GetScale().Y / 1.5f }); }

#pragma endregion


#pragma region Toast
		m_levelCompleteMenuObjects.ToastWord = Text::TextRenderer::RenderWordCaseSensitive("Toast:", Vec2{ 80.f, TargetResolutionY - 78.f }, 10.f, -1.75f, 1);

		m_levelCompleteMenuObjects.CollectedToast[0] = new AnimatingGUIElement(Vec2{ 114.f, TargetResolutionY - 78.f }, Vec2{ 16.f }, Renderer::GetSprite(GlobalLevelData::GetToastCollected(0) ? LEVEL_COMPLETE_MENU_TOAST_GOTTEN : LEVEL_COMPLETE_MENU_TOAST_MISSED));
		m_levelCompleteMenuObjects.CollectedToast[1] = new AnimatingGUIElement(Vec2{ 132.f, TargetResolutionY - 78.f }, Vec2{ 16.f }, Renderer::GetSprite(GlobalLevelData::GetToastCollected(1) ? LEVEL_COMPLETE_MENU_TOAST_GOTTEN : LEVEL_COMPLETE_MENU_TOAST_MISSED));
		m_levelCompleteMenuObjects.CollectedToast[2] = new AnimatingGUIElement(Vec2{ 150.f, TargetResolutionY - 78.f }, Vec2{ 16.f }, Renderer::GetSprite(GlobalLevelData::GetToastCollected(2) ? LEVEL_COMPLETE_MENU_TOAST_GOTTEN : LEVEL_COMPLETE_MENU_TOAST_MISSED));

		//Hide each toast for now, until animation plays.
		m_levelCompleteMenuObjects.CollectedToast[0]->SetScale(0.f);
		m_levelCompleteMenuObjects.CollectedToast[1]->SetScale(0.f);
		m_levelCompleteMenuObjects.CollectedToast[2]->SetScale(0.f);
		m_levelCompleteMenuObjects.CollectedToast[0]->SetOpacity(0.f);
		m_levelCompleteMenuObjects.CollectedToast[1]->SetOpacity(0.f);
		m_levelCompleteMenuObjects.CollectedToast[2]->SetOpacity(0.f);

		Renderer::LoadGUIElement(m_levelCompleteMenuObjects.CollectedToast[0]);
		Renderer::LoadGUIElement(m_levelCompleteMenuObjects.CollectedToast[1]);
		Renderer::LoadGUIElement(m_levelCompleteMenuObjects.CollectedToast[2]);
#pragma endregion

		m_levelCompleteMenuObjects.FinalScoreDelim = new StaticGUIElement(Vec2{ 75.f, TargetResolutionY - 86.f }, Vec2{ 185.f, 3.f }, Renderer::GetSprite(LEVEL_COMPLETE_MENU_DELIM_LINE_SPRITE));
		Renderer::LoadGUIElement(m_levelCompleteMenuObjects.FinalScoreDelim);

		m_levelCompleteMenuObjects.ScoreText = Text::TextRenderer::RenderWordCaseSensitive("Score:", Vec2{ 80.f, TargetResolutionY - 114.f }, 10.f, -1.75f, 1);

		GlobalLevelData::Score score = GlobalLevelData::TabulateScore();
		int scoreSprite = m_scoreSprites.at(score);

		m_levelCompleteMenuObjects.ScoreResult = new AnimatingGUIElement(Vec2{ 118.f, TargetResolutionY - 114.f },
			Vec2{ (score == GlobalLevelData::Score::AP || score == GlobalLevelData::Score::FF) ? (-24.f * (57.f/32.f) /*A+ and F- are larger sprites.*/) : -24.f, 24.f}, 
			Renderer::GetSprite(scoreSprite));

		//Move to account for negative scale.
		m_levelCompleteMenuObjects.ScoreResult->SetPosition(Vec2{ m_levelCompleteMenuObjects.ScoreResult->GetPosition().X - m_levelCompleteMenuObjects.ScoreResult->GetScale().X, m_levelCompleteMenuObjects.ScoreResult->GetPosition().Y });

		//Since position changed.
		m_levelCompleteMenuObjects.ScoreResult->SetAnimationParamsToCurrent();

		//Hide until animation plays.
		m_levelCompleteMenuObjects.ScoreResult->SetPosition(Vec2{ -m_levelCompleteMenuObjects.ScoreResult->GetScale().X, m_levelCompleteMenuObjects.ScoreResult->GetPosition().Y });
		m_levelCompleteMenuObjects.ScoreResult->SetOpacity(0.f);

		Renderer::LoadGUIElement(m_levelCompleteMenuObjects.ScoreResult);
		
	}

	inline void LevelCompleteMenu_RemoveGUIText(Text::TextRenderer::RenderedWord& word)
	{
		for (Text::TextRenderer::Letter letter : word)
		{
			Renderer::UnloadGUIElement(letter);
		}

		word.clear();
	}

	inline void LevelCompleteMenu_RemoveGUIText(Text::TextRenderer::ExpectedRenderedWord& word)
	{
		for (Text::TextRenderer::ExpectedLetter* letter : word)
		{
			letter->letterLock.lock();
			Renderer::UnloadGUIElement(letter->letter);
			letter->letterLock.unlock();
			delete letter;
		}

		word.clear();
	}

	inline void LevelCompleteMenu_RemoveGUINumber(Text::TextRenderer::RenderedDigit& number)
	{
		for (StaticGUIElement* digit : number)
		{
			Renderer::UnloadGUIElement(digit);
			delete digit;
		}

		number.clear();
	}

	inline void LevelCompleteMenu_RemoveGUINumber(AnimatingGUIElement::AnimatingNumber& number)
	{
		for (StaticGUIElement* digit : number)
		{
			Renderer::UnloadGUIElement(digit);
			delete digit;
		}

		number.clear();
	}

	void LevelCompleteMenu::closeMenu()
	{
		if (m_levelCompleteMenuObjects.Backing == nullptr) { return; }

		Renderer::UnloadGUIElement(m_levelCompleteMenuObjects.Backing);

		GUIManager::UnregisterButton(m_levelCompleteMenuObjects.ContinueButton);
		Renderer::UnloadGUIElement(m_levelCompleteMenuObjects.ContinueButton);

		LevelCompleteMenu_RemoveGUIText(m_levelCompleteMenuObjects.ContinueWord);

		LevelCompleteMenu_RemoveGUIText(m_levelCompleteMenuObjects.TimeWord);
		LevelCompleteMenu_RemoveGUINumber(m_levelCompleteMenuObjects.ElapsedMin);
		LevelCompleteMenu_RemoveGUINumber(m_levelCompleteMenuObjects.ElapsedSec);

		LevelCompleteMenu_RemoveGUIText(m_levelCompleteMenuObjects.PointsWord);
		LevelCompleteMenu_RemoveGUINumber(m_levelCompleteMenuObjects.PointsAmount);

		Renderer::UnloadGUIElement(m_levelCompleteMenuObjects.CollectedToast[0]);
		Renderer::UnloadGUIElement(m_levelCompleteMenuObjects.CollectedToast[1]);
		Renderer::UnloadGUIElement(m_levelCompleteMenuObjects.CollectedToast[2]);

		Renderer::UnloadGUIElement(m_levelCompleteMenuObjects.FinalScoreDelim);

		LevelCompleteMenu_RemoveGUIText(m_levelCompleteMenuObjects.ScoreText);
		Renderer::UnloadGUIElement(m_levelCompleteMenuObjects.ScoreResult);

		Collectables::CollectableTracker::UpdateCurrencyAtEnd();

		TestGame::SetLoadLevelWithSavedPlayer(false);
		TestGame::INSTANCE->LoadLevelAndAllData(m_levelDestination.c_str());
	}
}