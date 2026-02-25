#include "TextRenderer.h"

#include "../../../Rendering/Renderers/Renderer.h"

#include "../../../EngineCompileOptions.h"
#include <algorithm>

#include "../../../Utils/Math/VMath.h"

#include "../../../Input/InputManager.h"


namespace GAME_NAME::Objects::GUI::Text
{

	StaticGUIElement* TextRenderer::RenderDigit(digit digit, Vec2& position, const float scale, int font)
	{
		int texture = font + (int)digit;
		StaticGUIElement* go = new StaticGUIElement(position, Vec2(-TEXT_RENDERER_DIGIT_SIZE_X * scale, TEXT_RENDERER_DIGIT_SIZE_Y * scale), Renderer::GetSprite(texture)->GetSpriteId());

		Renderer::LoadGUIElement(go, 1);

		return go;
	}

	GAME_NAME::Objects::GUI::Text::TextRenderer::RenderedDigit TextRenderer::RenderNumber(uint16_t number, Vec2 firstDigitPosition, const float scale, const float digitPadding, uint8_t minimumDigits, int font)
	{
		RenderedDigit finalDigit;
		finalDigit.reserve(num_digitsi(number));

		uint8_t currentDigit = 0;
		while (number > 0)
		{
			for (StaticGUIElement* sge : finalDigit)
			{
				sge->SetPosition(sge->GetPosition() + Vec2(TEXT_RENDERER_DIGIT_SIZE_X * scale + digitPadding, 0));
			}

			Vec2 pos(firstDigitPosition.X, firstDigitPosition.Y);
		
			finalDigit.push_back(RenderDigit(number % 10, pos, scale, font));

			number /= 10;
			currentDigit++;
			if(minimumDigits > 0){ minimumDigits--; }
		}

		while (minimumDigits > 0)
		{
			for (StaticGUIElement* sge : finalDigit)
			{
				sge->SetPosition(sge->GetPosition() + Vec2(TEXT_RENDERER_DIGIT_SIZE_X * scale + digitPadding, 0));
			}

			Vec2 pos(firstDigitPosition.X, firstDigitPosition.Y);
			finalDigit.push_back(RenderDigit(0, pos, scale, font));
			minimumDigits--;
		}
		
		return finalDigit;
	}


	constexpr std::tuple<char, int> SPECIAL_CHARACTERS[8] = {
		std::tuple<char, int>('.', DEFAULT_FONT_RENDER_LOWERCASE_A_SPRITE_ID + 26),
		std::tuple<char, int>(',', DEFAULT_FONT_RENDER_LOWERCASE_A_SPRITE_ID + 27),
		std::tuple<char, int>('!', DEFAULT_FONT_RENDER_LOWERCASE_A_SPRITE_ID + 28),
		std::tuple<char, int>('?', DEFAULT_FONT_RENDER_LOWERCASE_A_SPRITE_ID + 29),
		std::tuple<char, int>('[', DEFAULT_FONT_RENDER_LOWERCASE_A_SPRITE_ID + 30),
		std::tuple<char, int>(']', DEFAULT_FONT_RENDER_LOWERCASE_A_SPRITE_ID + 31),
		std::tuple<char, int>('-', DEFAULT_FONT_RENDER_LOWERCASE_A_SPRITE_ID + 32),
		std::tuple<char, int>(':', SpriteBase(270))
	};


	TextRenderer::Letter TextRenderer::RenderLetter(char letter, const Vec2& position, const float scale, int layer, int font)
	{
		int texture = GLOBAL_SPRITE_BASE;

		for (int i = 0; i < VALID_CHARS_LENGTH; i++)
		{
			if (letter >= m_validWordChars[i].StartLetter && letter <= m_validWordChars[i].EndLetter)
			{
				texture = font + m_validWordChars[i].Offset + (letter - m_validWordChars[i].StartLetter);
				break;
			}
		}

		if (texture == GLOBAL_SPRITE_BASE)
		{
			for (std::tuple<char, int> charMap : SPECIAL_CHARACTERS)
			{
				if (std::get<0>(charMap) == letter)
				{
					texture = std::get<1>(charMap);
				}
			}

			if (texture == GLOBAL_SPRITE_BASE)
			{
				return nullptr;
			}
		}

		StaticGUIElement* GUIelement = new StaticGUIElement(position, { -scale, scale }, Renderer::GetSprite(texture)->GetSpriteId());
		Letter letterObject(GUIelement);

		Renderer::LoadGUIElement(GUIelement, layer);

		return letterObject;
	}

	TextRenderer::RenderedWord TextRenderer::RenderWord(std::string word, Vec2 position, const float scale, const float&& letterPadding, int layer, int font)
	{
		//Convert entire word string to lowercase.
		std::transform(word.begin(), word.end(), word.begin(), [](unsigned char l) {
			return std::tolower(l);
		});

		RenderedWord wordRet;
		wordRet.reserve(word.size());

		for (size_t i = 0; i < word.size(); i++)
		{
			Letter letter = RenderLetter(word.at(i), position + Vec2{ i * (scale + letterPadding), 0 }, scale, layer, font);
			if (letter == nullptr) { continue; }
			wordRet.push_back(letter);
		}
		
		return wordRet;
	}

	//How much tighter to fit the letter spacing based on what lowercase letter is typed.
	constexpr float LOWERCASE_LETTER_SPACING[26] = {
		0.f,		//a
		0.05f,		//b
		0.021f,		//c
		0.05f,		//d
		0.02f,		//e
		0.f,		//f
		0.f,		//g
		0.f,		//h
		0.23f,		//i
		0.15f,		//j
		0.05f,		//k
		0.2f,		//l
		0.f,		//m
		0.03f,		//n
		0.04f,		//o
		0.f,		//p
		0.f,		//q
		0.065f,		//r
		0.f,		//s
		0.2f,		//t
		0.01f,		//u
		0.064f,		//v
		0.f,		//w
		0.f,		//x
		0.005f,		//y
		0.f			//z
	};

	//How far the position of the letter should be shifted down below other letters.
	constexpr float LOWERCASE_LETTER_SHIFT[26] = {
		0.f,		//a
		0.f,		//b
		0.f,		//c
		0.f,		//d
		0.f,		//e
		0.f,		//f
		0.15f,		//g
		0.f,		//h
		0.f,		//i
		0.15f,		//j
		0.f,		//k
		0.f,		//l
		0.f,		//m
		0.f,		//n
		0.f,		//o
		0.13f,		//p
		0.13f,		//q
		0.f,		//r
		0.f,		//s
		0.f,		//t
		0.f,		//u
		0.f,		//v
		0.f,		//w
		0.f,		//x
		0.08f,		//y
		0.f			//z
	};

	TextRenderer::ExpectedRenderedWord TextRenderer::RenderWordCaseSensitive(std::string word, Vec2 position, const float scale, const float& letterPadding, int layer, int uppercaseFont, int lowercaseFont, std::chrono::milliseconds letterAppearanceSpeed)
	{
		ExpectedRenderedWord wordRet;
		wordRet.reserve(word.size());

		Vec2 currentPosition = position;

		for (size_t i = 0; i < word.size(); i++)
		{

			if (word.at(i) == '\n')
			{
				currentPosition.Y -= scale + 1;
				currentPosition.X = position.X;
				continue;
			}

			if (std::isupper(word.at(i)))
			{
				Letter letter = RenderLetter(std::tolower(word.at(i)), currentPosition, scale, layer, uppercaseFont);
				currentPosition += Vec2{ (scale + letterPadding), 0 };
				if (letter == nullptr) { continue; }
				
				ExpectedLetter* l = new ExpectedLetter(std::mutex(), letter);
				l->letter->SetScale(0);
				wordRet.push_back(l);
			}
			else {

				int letterSpacingIndex = word.at(i) - 'a';

				Vec2 downShift = 0;

				if (letterSpacingIndex < 25 && letterSpacingIndex >= 0)
				{
					currentPosition.X -= LOWERCASE_LETTER_SPACING[letterSpacingIndex] * scale;
					downShift.Y -= LOWERCASE_LETTER_SHIFT[letterSpacingIndex] * scale;
				}

				Letter letter = RenderLetter(std::tolower(word.at(i)), currentPosition + downShift, scale, layer, lowercaseFont);
				currentPosition += Vec2{ (scale + letterPadding), 0};

				if (letterSpacingIndex < 25 && letterSpacingIndex >= 0)
				{
					currentPosition.X -= LOWERCASE_LETTER_SPACING[letterSpacingIndex] * scale;
				}

				if (letter == nullptr) { continue; }

				ExpectedLetter* l = new ExpectedLetter(std::mutex(), letter);
				l->letter->SetScale(0);
				wordRet.push_back(l);
			}
		}

		std::thread([](ExpectedRenderedWord word, std::chrono::milliseconds speed, const float expectedScale) {
			for (int i = 0; i < word.size(); i++)
			{
				//Potentially issues, but used later to check if dialogue can be advanced. Lock mutex here.
				word[i]->letterLock.lock();

				//Right click to speed up text.
				if (!InputManager::GetMouseButton(1))
				{
					std::this_thread::sleep_for(speed);
				}
				else {
					std::this_thread::sleep_for(std::chrono::milliseconds(2));
				}

				word[i]->letter->SetScale(Vec2{ -expectedScale, expectedScale });

				word[i]->letterLock.unlock();

				if (speed != speed.zero())
				{
					std::thread([](ExpectedLetter* letter) {
						letter->letterLock.lock();

						letter->letter->SetPosition(letter->letter->GetPosition() + Vec2{ 0, 3 });

						std::this_thread::sleep_for(std::chrono::milliseconds(5));

						letter->letter->SetPosition(letter->letter->GetPosition() + Vec2{ 0, -1 });

						std::this_thread::sleep_for(std::chrono::milliseconds(3));

						letter->letter->SetPosition(letter->letter->GetPosition() + Vec2{ 0, -1 });

						std::this_thread::sleep_for(std::chrono::milliseconds(3));

						letter->letter->SetPosition(letter->letter->GetPosition() + Vec2{ 0, -1 });

						letter->letterLock.unlock();
					}, word[i]).detach();
				}
				

			}

			
		}, wordRet, letterAppearanceSpeed, scale).detach();

		return wordRet;
	}

	void TextRenderer::UnloadWord(ExpectedRenderedWord word, int layer)
	{
		for (ExpectedLetter* letter : word)
		{
			letter->letterLock.lock();
			Renderer::UnloadGUIElement(letter->letter, layer);
			letter->letterLock.unlock();

			delete letter;
		}
	}


}