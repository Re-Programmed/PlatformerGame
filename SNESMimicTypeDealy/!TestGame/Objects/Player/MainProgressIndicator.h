#pragma once
#include "../../../Objects/GUI/Progress/ProgressBar.h"

namespace GAME_NAME::Objects::Player
{
	/// <summary>
	/// Progress bar at the bottom of the screen to show progress of various ingame events.
	/// </summary>
	class MainProgressIndicator
	{
	public:
		/// <summary>
		/// Update the percentage (/1.0) for the progress bar. Will create one if needed and destroy the existing one if it is filled.
		/// </summary>
		/// <param name="progress"></param>
		static void UpdateProgressIndicator(double progress);
	private:
		static std::unique_ptr<GAME_NAME::Objects::GUI::ProgressBar> m_progressBar;
	};
}