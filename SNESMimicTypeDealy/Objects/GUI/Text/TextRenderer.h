#pragma once

#include <string>
#include "../../../Utils/Math/Vec2.h"
#include "../../../Rendering/Sprite.h"
#include <vector>
#include "../../../Objects/GUI/StaticGUIElement.h"

#define VALID_CHARS_LENGTH 1

namespace GAME_NAME::Objects::GUI::Text
{
	using namespace MathUtils;

	/*
		Invalid letters are rendered as a space.
	*/

	/// <summary>
	/// A static class used to render letters and numbers.
	/// </summary>
	class TextRenderer
	{
	public:
		typedef unsigned char digit;

		typedef std::vector<StaticGUIElement*> RenderedDigit;

		/// <summary>
		/// A letter is a GUI element.
		/// </summary>
		typedef StaticGUIElement* Letter;
		/// <summary>
		/// A word is a vector of letters.
		/// </summary>
		typedef std::vector<Letter> RenderedWord;

		struct ExpectedLetter {
			std::mutex letterLock;
			Letter letter;


		};


		/// <summary>
		/// Not all the letters in an ExpectedRendereedWord have appeared yet, the letters that haven't appeared will have a scale of 0, so if they are modified the mutex letterLock must be avaiable to avoid conflicts.
		/// </summary>
		typedef std::vector<ExpectedLetter*> ExpectedRenderedWord;

		/// <summary>
		/// Draws a digit to the screen at the given position with the given scale.
		/// </summary>
		/// <param name="digit">Digit must be less than 10.</param>
		/// <param name="position">The position to draw the digit.</param>
		/// <param name="scale">The scale to draw the digit.</param>
		/// <returns></returns>
		static StaticGUIElement* RenderDigit(digit digit, Vec2& position, const float scale, int font = TEXT_RENDERER_ZERO_DIGIT_SPRITE_ID);

		static RenderedDigit RenderNumber(uint16_t number, Vec2& firstDigitPosition, const float scale, const float digitPadding, uint8_t minimumDigits = 0, int font = TEXT_RENDERER_ZERO_DIGIT_SPRITE_ID);

		/// <summary>
		/// Renders the given char as a texture offset from the "DEFAULT_FONT_RENDER_A_SPRITE_ID."
		/// Returns nullptr if no sprite was rendered (invalid letter or ' ')
		/// </summary>
		/// <param name="letter"></param>
		/// <param name="position"></param>
		/// <param name="scale"></param>
		/// <returns></returns>
		static Letter RenderLetter(char letter, const Vec2& position, const float scale, int layer = 1, int font = DEFAULT_FONT_RENDER_A_SPRITE_ID);

		/// <summary>
		/// Renders a word with given scale, position, and letterPadding based on texture offset from "DEFAULT_FONT_RENDER_A_SPRITE_ID."
		/// </summary>
		/// <param name="word">[string] - The word to render.</param>
		/// <param name="position">[Vec2] - Where to place the first letter of the word.</param>
		/// <param name="scale">[const float] - The size of all the letters.</param>
		/// <param name="letterPadding">[const float&amp;&amp;] - The space between each letter.</param>
		/// <returns>[RenderedWord] - The word that was rendered.</returns>
		static RenderedWord RenderWord(std::string word, Vec2 position, const float scale, const float&& letterPadding, int layer = 1, int font = DEFAULT_FONT_RENDER_A_SPRITE_ID);

		static ExpectedRenderedWord RenderWordCaseSensitive(std::string word, Vec2 position, const float scale, const float&& letterPadding, int layer = 1, int uppercaseFont = DEFAULT_FONT_RENDER_A_SPRITE_ID, int lowercaseFont = DEFAULT_FONT_RENDER_LOWERCASE_A_SPRITE_ID, std::chrono::milliseconds letterAppearanceSpeed = std::chrono::milliseconds(0));

		/// <summary>
		/// Unloads and deletes all letters of the provided word.
		/// </summary>
		/// <param name="word">[ExpectedRenderedWord] - The word to delete.</param>
		/// <param name="layer">[int] - The layer the word is on.</param>
		static void UnloadWord(ExpectedRenderedWord word, int layer = 1);
	private:
		/// <summary>
		/// Character ranges that can be rendered by render word.
		/// </summary>
		inline static const struct {
			char StartLetter, EndLetter;
			uint8_t Offset;
		} m_validWordChars[VALID_CHARS_LENGTH] = {
			{ 'a', 'z', 0 }
		};
	};
}