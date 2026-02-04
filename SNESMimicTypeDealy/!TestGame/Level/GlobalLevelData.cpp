#include "GlobalLevelData.h"
#include "./LevelInfoDisplay.h"

#include "../../Resources/AssetManager.h"
#include "../TestGame.h"

#include "../../Utils/Time/GameTime.h"

namespace GAME_NAME::Level
{
	GlobalLevelData::ConcurrentLevelData GlobalLevelData::m_concurrentLevelData;

	GlobalLevelData::DesiredLevelStats GlobalLevelData::LevelGoals = { 0 };

	std::vector<GameObject*> GlobalLevelData::m_scoreDisplayObjects;

	/*
		Level Stats File:
			0 - Min Time Elapsed,
			1 - Reasonable Number of Points
	*/

	void GlobalLevelData::ReadDesiredLevelStats()
	{
		unsigned int secondsElapsed = LevelGoals.SecondsElapsed;
		int desiredPoints = LevelGoals.MaxReasonablePoints;

		std::string goal = Resources::AssetManager::ReadLevelGoal(TestGame::INSTANCE->GetCurrentLevelPath().c_str(), 0);
		if (goal != "")
		{
			if (goal.ends_with("s")) { goal = goal.substr(0, goal.length() - 1); }
			secondsElapsed = std::stoi(goal);
		}

		goal = Resources::AssetManager::ReadLevelGoal(TestGame::INSTANCE->GetCurrentLevelPath().c_str(), 1);
		if (goal != "")
		{
			desiredPoints = std::stoi(goal);
		}

		LevelGoals = DesiredLevelStats{ secondsElapsed, desiredPoints };
	}

	GlobalLevelData::Score GlobalLevelData::TabulateScore()
	{
		unsigned int pointCalculation = 0;

		//TODO: Figure out score some smarter way.

		int elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - m_concurrentLevelData.LevelStartTime.count();

		//Get 10 points for completing the level under the specified time. 

		if (elapsedSeconds <= LevelGoals.SecondsElapsed)
		{
			pointCalculation += 10;
		}
		else {
			//Get 1 less point for every half time you went over.
			int pointsLost = (elapsedSeconds / (LevelGoals.SecondsElapsed / 2)) - 2;
			if (pointsLost < 0) { pointsLost = 0; }

			pointCalculation += (10 - pointsLost);	
		}

		if (m_concurrentLevelData.Points >= LevelGoals.MaxReasonablePoints)
		{
			pointCalculation += 10;
		}
		else {
			//Gain one point for every 1/10 of the max reasonable points (minus 1).
			int pointsGained = ((10 * m_concurrentLevelData.Points) / LevelGoals.MaxReasonablePoints) - 1;
			if (pointsGained < 0) { pointsGained = 0; }

			pointCalculation += pointsGained;
		}

		//2 points for each toast.
		if (m_concurrentLevelData.ToastCollected[0]) { pointCalculation += 2; }
		if (m_concurrentLevelData.ToastCollected[1]) { pointCalculation += 2; }
		if (m_concurrentLevelData.ToastCollected[2]) { pointCalculation += 2; }

		//Arbitrary numbers...
		if (pointCalculation >= 26 /*Perfect*/)
		{
			return Score::AP;
		}
		else if (pointCalculation >= 20)
		{
			return Score::A;
		}
		else if (pointCalculation >= 16)
		{
			return Score::B;
		}
		else if (pointCalculation >= 12)
		{
			return Score::C;
		}
		else if (pointCalculation >= 6)
		{
			return Score::D;
		}
		else if (pointCalculation >= 2)
		{
			return Score::F;
		}
		else
		{
			return Score::FF;
		}
	}

	GlobalLevelData::GlobalLevelData(std::string levelPath)
		: MiscStateGroup("gld"), m_levelPath(levelPath)
	{
		//Seems like a good amount.
		m_scoreDisplayObjects.reserve(10);

		auto states = getStates();
		if (states->size() > 0)
		{
			m_concurrentLevelData.Decode((*states)[0]);
		}

		assignState(&m_concurrentLevelData);
	}

	std::unordered_map<int, int> GlobalLevelData_ScorePointValueSprites = {
		{ 1, SpriteBase(280) },
		{ 5, SpriteBase(281) },
		{ 10, SpriteBase(282) },
		{ 20, SpriteBase(283) },
		{ 50, SpriteBase(284) },
		{ 100, SpriteBase(285) }
	};

	void GlobalLevelData::UpdatePoints(int16_t pointChange)
	{
		m_concurrentLevelData.Points += pointChange;
		

		/*
		LevelInfoDisplay::UpdateDSP(m_points);*/
	}

	void GlobalLevelData::UpdatePoints(int16_t pointChange, Vec2 displayGraphicPosition)
	{
		m_concurrentLevelData.Points += pointChange;

		int closestPointValueDistance = 10000, closestSprite = SpriteBase(285);
		for (auto [points, sprite] : GlobalLevelData_ScorePointValueSprites)
		{
			int distance = std::abs(points - pointChange);
			if (distance < closestPointValueDistance)
			{
				closestPointValueDistance = distance;
				closestSprite = sprite;
			}
		}

		GlobalLevelData::ScoreObject* scoreObject = new GlobalLevelData::ScoreObject(displayGraphicPosition, Renderer::GetSprite(closestSprite));
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(scoreObject, false, 2, true));
	}


	void GlobalLevelData::ConcurrentLevelData::Decode(const SaveParam param)
	{
		std::stringstream ss(param);
		std::string line;
		
		int i = 0;
		while (std::getline(ss, line, ';'))
		{
			switch (i++)
			{
			case 0:
				LevelStartTime = std::chrono::seconds(std::stoi(line)); break;
			case 1:
				Points = std::stoi(line); break;
			case 2:
				ToastCollected[0] = line.starts_with('t'); break;
			case 3:
				ToastCollected[1] = line.starts_with('t'); break;
			case 4:
				ToastCollected[2] = line.starts_with('t'); break;
			}
		}

	}

	MiscState::SaveParam GlobalLevelData::ConcurrentLevelData::Encode()
	{
		SaveParam param("");
		param.append(std::to_string(LevelStartTime.count())).append(";")
			.append(std::to_string(Points)).append(";")
			.append(ToastCollected[0] ? "t" : "f").append(";")
			.append(ToastCollected[1] ? "t" : "f").append(";")
			.append(ToastCollected[2] ? "t" : "f").append(";");
		return param;
	}

	constexpr double ScoreObject_MaxLifetime = 1.5;

	void GlobalLevelData::ScoreObject::Render(const Vec2& cameraPos)
	{
		float opacity = 1.f - static_cast<float>(m_lifetime / ScoreObject_MaxLifetime);

		Vec4 colors[4] = {
			{ 1.f, 1.f, 1.f, opacity },
			{ 1.f, 1.f, 1.f, opacity },
			{ 1.f, 1.f, 1.f, opacity },
			{ 1.f, 1.f, 1.f, opacity }
		};

		DynamicSprite sprite(m_sprite->GetSpriteId());
		sprite.UpdateTextureColor(colors);
		sprite.Render(cameraPos, m_position + Vec2{ 0.f, m_scale.Y }, m_scale * Vec2{ 1.f, -1.f }, 0.f);
	}

	void GlobalLevelData::ScoreObject::Update(GLFWwindow* window)
	{
		m_lifetime += Utils::Time::GameTime::GetScaledDeltaTime();

		Translate(m_randomVelocity * Utils::Time::GameTime::GetScaledDeltaTime());

		if (m_lifetime >= ScoreObject_MaxLifetime)
		{
			Renderer::DestroyObject(this);
		}
	}

}
