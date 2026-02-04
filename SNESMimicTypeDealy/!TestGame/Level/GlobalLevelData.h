#pragma once
#include <cstdint>
#include <string>

#include <chrono>

#include "../../Objects/MiscStateGroup.h"

namespace GAME_NAME::Level
{
	/// <summary>
	/// Used to track data about the current level that will change based on actions throughout the level.
	/// </summary>
	class GlobalLevelData
		: public MiscStateGroup
	{
	public:
		enum class Score
		{
			AP,	//A+
			A,	//A
			B,	//B
			C,	//C
			D,	//D
			F,	//F
			FF	//F-
		};

		static Score TabulateScore();

		GlobalLevelData(std::string levelPath);

		/// <summary>
		/// Updates the current amount of points the player(s) has/have gotten and all related graphical displays.
		/// </summary>
		/// <param name="pointChange">How much to increase/decrease the current points by.</param>
		static void UpdatePoints(int16_t pointChange);

		static inline uint16_t GetPoints() { return m_concurrentLevelData.Points; }

		static inline std::chrono::seconds GetLevelStartTime() { return m_concurrentLevelData.LevelStartTime; }

		static inline void SetLevelStartTime(std::chrono::seconds time)
		{
			m_concurrentLevelData.LevelStartTime = time;
		}

		static inline void CollectToast(uint8_t toastID)
		{
			m_concurrentLevelData.ToastCollected[toastID] = true;
		}

		static inline bool GetToastCollected(uint8_t toastID)
		{
			return m_concurrentLevelData.ToastCollected[toastID];
		}

		/// <summary>
		/// Returns the relative file path to the current level's files.
		/// </summary>
		inline std::string GetLevelPath()
		{
			return m_levelPath;
		}

	private:
		class ConcurrentLevelData
			: public MiscState
		{
		public:
			std::chrono::seconds LevelStartTime SERIALIZED;

			/// <summary>
			/// The points gathered on the current level.
			/// </summary>
			uint16_t Points = 0 SERIALIZED;

			bool ToastCollected[3] = { false, false, false } SERIALIZED;

			void Decode(const SaveParam param) override;
			SaveParam Encode() override;
		};

		//Things that are not based on the current actual level, but a global scope.
		static ConcurrentLevelData m_concurrentLevelData;


		/// <summary>
		/// The relative file path to the level that was loaded.
		/// </summary>
		const std::string m_levelPath;
	};
}