#pragma once

#include "../StaticGUIElement.h"

namespace GAME_NAME::Objects::GUI
{

	using namespace MathUtils;

	/// <summary>
	/// A GUI element that can be cut off at the end based on a given percentage out of 100.
	/// </summary>
	class ProgressBar
		: public StaticGUIElement
	{
	public:
		ProgressBar(Vec2 position, Vec2 scale, GLuint texture)
			:StaticGUIElement(position, scale, texture)
		{
			SetPercentage(100);
		}

		ProgressBar(Vec2 position, Vec2 scale, GLuint texture, char percentage)
			:StaticGUIElement(position, scale, texture)
		{
			SetPercentage(percentage);
		}

		/// <summary>
		/// Sets the current percentage (0 - 100) of the progress bar that is not cut off.
		/// </summary>
		/// <param name="percentage">[char] - The percentage of the progress bar visible from the left.</param>
		/// <param name="ignoreSmoothing">[bool] - Weather to perform a smoothing function on the progress bar while updating.</param>
		void SetPercentage(char percentage, bool ignoreSmoothing = false);

		inline char GetPercentage()
		{
			return m_percentage;
		}

		/// <summary>
		/// When the bar is animating to a new position, the display percentage will be changing differently than the actual value of the percentage.
		/// </summary>
		/// <returns></returns>
		inline float GetDisplayPercentage()
		{
			return m_displayPercentage;
		}

		void Render(float zoom) override;

	private:
		/// <summary>
		/// The percentage displayed, used for smoothing the display percentage to lerp toward the target percentage.
		/// </summary>
		float m_displayPercentage;
		/// <summary>
		/// The current percentage displayed by this progress bar.
		/// </summary>
		char m_percentage;
	};
}