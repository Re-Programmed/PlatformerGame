#include "GlobalLevelData.h"
#include "./LevelInfoDisplay.h"

namespace GAME_NAME::Level
{
	GlobalLevelData::ConcurrentLevelData GlobalLevelData::m_concurrentLevelData;

	GlobalLevelData::Score GlobalLevelData::TabulateScore()
	{
		//TODO: Figure out score.
		return Score::AP;
	}

	GlobalLevelData::GlobalLevelData(std::string levelPath)
		: MiscStateGroup("gld"), m_levelPath(levelPath)
	{
		auto states = getStates();
		if (states->size() > 0)
		{
			m_concurrentLevelData.Decode((*states)[0]);
		}

		assignState(&m_concurrentLevelData);
	}

	void GlobalLevelData::UpdatePoints(int16_t pointChange)
	{
		m_concurrentLevelData.Points += pointChange;

		/*
		//TODO: Update graphical display for points.
		LevelInfoDisplay::UpdateDSP(m_points);*/
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

}
