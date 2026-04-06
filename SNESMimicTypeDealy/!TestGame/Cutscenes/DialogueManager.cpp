#include "DialogueManager.h"

#include "../../Input/InputManager.h"

#include "../../Resources/AssetManager.h"

#include "../Camera/GameCamera.h"

#include "../../Objects/GUI/GUIManager.h"

#include "../../Audio/SoundEvents.h"

#include "../Items/ItemType.h"

#include "./Character/AnimatingCharacter.h"

#include <thread>
#include <regex>

#include "../../Utils/Time/GameTime.h"

#include "../../CustomCompileOptions.h"


/*-----------------------------------------
Items can be templated in dialogue files using "%item%".
-----------------------------------------*/

// The render text function used by text for dialogue events.
#define DialogueManager_RenderDialogue(text) DialogueManager::RenderCaseSensitiveTextWithItemTextures(text, { DIALOGUE_TEXT_BOX_SPACING_X + 12, DIALOGUE_TEXT_BOX_SPACING_Y + 14 }, 9, -3, 1, DEFAULT_FONT_RENDER_A_SPRITE_ID, DEFAULT_FONT_RENDER_LOWERCASE_A_SPRITE_ID, std::chrono::milliseconds(62))


#define ITEM_TEXTURE_INDICATOR_PREFIX "%item_"
#define ITEM_TEXTURE_INDICATOR_PREFIX_LEN 6

namespace GAME_NAME::Cutscenes
{
	DialogueManager* DialogueManager::INSTANCE = nullptr; //Singleton.
	bool DialogueManager::m_jigglingText = false;		  //Used to track if the text jiggle animation is active to prevent it playing over top of itself.

	DialogueSequence::DialogueEvent DialogueManager::m_currentDialogueEvent("");	//The current dialogue event that is getting displayed.

	std::array<GUIButton*, 3> DialogueManager::m_guiOptionButtons;	//If the current event is a multiple selection of buttons, this array will populate with the button option objects. (used to delete the buttons when finished)

	void DialogueManager::createDialogueBox(int textBoxTexture)
	{
		if (!m_playingDialogueSequence) { return; }

		//A textbox already exists, failure.
		if (std::get<0>(m_guiDisplay) != nullptr) { return; }

		//Create a textbox background.
		auto sp = Renderer::GetSprite(textBoxTexture);
		StaticGUIElement* textBox = new StaticGUIElement({ DIALOGUE_TEXT_BOX_SPACING_X, DIALOGUE_TEXT_BOX_SPACING_Y }, { TargetResolutionX - (2 * DIALOGUE_TEXT_BOX_SPACING_X), 25}, sp->GetSpriteId());
		Renderer::LoadGUIElement(textBox);

		//Initilize the current dialogue.
		m_currentDialogueEvent = m_currentDialogueSequence.Next();

		//If the next dialogue is not a selection, render the text; otherwise, create the selection buttons.
		if (m_currentDialogueEvent.Options.empty())
		{
			Text::TextRenderer::ExpectedRenderedWord erw = DialogueManager_RenderDialogue(m_currentDialogueEvent.Text);
			m_guiDisplay = std::tuple<StaticGUIElement*, Text::TextRenderer::ExpectedRenderedWord>(textBox, erw);
		}
		else {
			createDialogueOptions();
		}

			//--- Handle camera --//

		updateDialogueEvents();

	}

	//Used to track if the mouse is pressed to prevent dialogue from skipping.
	bool advancingDialogue = false;

	unsigned int numLettersShown = 0;

	/// <summary>
	/// Find instances of ITEM_TEXTURE_INDICATOR_PREFIX and replace them with their corresponding item texture display. Then, render a text object with these replacements.
	/// </summary>
	/// <param name="word"></param>
	/// <param name="position"></param>
	/// <param name="scale"></param>
	/// <param name="letterPadding"></param>
	/// <param name="layer"></param>
	/// <param name="uppercaseFont"></param>
	/// <param name="lowercaseFont"></param>
	/// <param name="letterAppearanceSpeed"></param>
	/// <returns></returns>
	std::vector<Text::TextRenderer::ExpectedLetter*> DialogueManager::RenderCaseSensitiveTextWithItemTextures(std::string word, Vec2 position, const float scale, const float& letterPadding, int layer, int uppercaseFont, int lowercaseFont, std::chrono::milliseconds letterAppearanceSpeed)
	{
		std::unordered_map<size_t, ITEM_TYPE> encodedItems;
		std::string itemEncodedText = m_currentDialogueEvent.Text;

		for (int i = 0; i < itemEncodedText.size(); i++)
		{
			//Find instances of "%item_"
			std::string compString = itemEncodedText.substr(i, ITEM_TEXTURE_INDICATOR_PREFIX_LEN);
			if (compString == ITEM_TEXTURE_INDICATOR_PREFIX)
			{
				//Find where the end of the item string is, which will be the next space.
				size_t spaceAfter = i + itemEncodedText.substr(i).find_first_of(' ');

				//We have to offset the index by how many spaces are before this character as the spaces are not included as objects in the final rendered word.
				unsigned int numSpacesBefore = std::count(itemEncodedText.begin(), itemEncodedText.begin() + i, ' ');
				//Append the proper index and corresponding item type.
				encodedItems.emplace(i - numSpacesBefore, static_cast<ITEM_TYPE>(std::stoi(itemEncodedText.substr(i + ITEM_TEXTURE_INDICATOR_PREFIX_LEN, spaceAfter - i))));

				//Update the text to be rendered so that the place where the item will go is just a single period.
				itemEncodedText = itemEncodedText.substr(0, i).append(".").append(itemEncodedText.substr(spaceAfter));
			}
		}

		Text::TextRenderer::ExpectedRenderedWord erw = Text::TextRenderer::RenderWordCaseSensitive(itemEncodedText, position, scale, letterPadding, layer, uppercaseFont, lowercaseFont, letterAppearanceSpeed);

		for (auto& [index, itemType] : encodedItems)
		{
			erw[index]->letter->SetSprite(ITEMTYPE_GetItemTypeTexture(itemType));
		}

		return erw;
	}

	void DialogueManager::Update(GLFWwindow* window)
	{
		//Currently selecting a button, do not allow advancing.
		if (DialogueManager::m_currentDialogueEvent.Options.size() > 0) { return; }

		//Maybe too many ifs...
		if (m_playingDialogueSequence)
		{
			{
				unsigned int numLetters = 0;
				//Check if the number of letters shown is increasing.
				for (Text::TextRenderer::ExpectedLetter* letter : std::get<1>(m_guiDisplay))
				{
					if (letter->letterLock.try_lock())
					{
						numLetters++;
						letter->letterLock.unlock();
					}
					else
					{
						break;
					}
				}

				if (numLetters != numLettersShown)
				{
					Audio::SoundEvents::PlaySoundGlobal(Audio::SoundEvents::Event::DIALOGUE_SPEAKING_LOW, 0.32f);
					numLettersShown = numLetters;
				}
			}

			if (InputManager::GetMouseButton(0) || InputManager::GetControllerButton(0, controllerRef::BUTTON_A) || InputManager::GetControllerButton(0, controllerRef::BUTTON_B) || InputManager::GetControllerButton(0, controllerRef::BUTTON_ZL) || InputManager::GetControllerButton(0, controllerRef::BUTTON_ZR) || InputManager::GetControllerButton(0, controllerRef::BUTTON_L) || InputManager::GetControllerButton(0, controllerRef::BUTTON_R))
			{
				if (!advancingDialogue)
				{
					if (!advanceDialogue() && !m_jigglingText)
					{
						m_jigglingText = true; //100% JIGGLE TO THE MAX
						
						//Jiggle animation via thread: 
						std::thread t = std::thread([](Text::TextRenderer::ExpectedRenderedWord gui) {

							for (int i = 0; i < 4; i++)
							{
								for (int j = 0; j < gui.size(); j++)
								{
									Text::TextRenderer::ExpectedLetter* letter = gui[j];

									letter->letterLock.lock();
									letter->letter->SetPosition(letter->letter->GetPosition() + Vec2{ 0, ((i + j) % 2 == 0) ? 1.3f : -1.3f});
									letter->letterLock.unlock();
								}

								std::this_thread::sleep_for(std::chrono::milliseconds(3));
							}

							m_jigglingText = false;

						}, std::get<1>(m_guiDisplay));

						t.detach();
					}

					advancingDialogue = true;
				}
			}
			else {
				advancingDialogue = false;
			}

			//Update to ensure the speaker display sprite matches the target object sprite.
			updateSpeakerDisplay();
		}
	}

	DialogueSequence DialogueManager::GetDialogueSequence(std::string sequenceName, ITEM_TYPE refItem)
	{
		if (m_storedDialogueSequences.contains(sequenceName))
		{
			std::stringstream ss(m_storedDialogueSequences[sequenceName]);
			std::string line;

			std::vector<DialogueSequence::DialogueEvent> events;
			while (std::getline(ss, line, '&'))
			{
				float zoom = 1.f;

				//Asterisk means zoom value.
				if (line.starts_with("*"))
				{
					std::string zoomStr = line.substr(1, line.find_last_of('*'));
					zoom = std::stof(zoomStr);

					line = line.substr(line.find_last_of("*") + 1);
				}

				//Options deliniated by pipe. "|"
				if (line.find("|") != std::string::npos)
				{
					DialogueSequence::DialogueEvent event(line.substr(0, line.find_first_of('|')), nullptr, zoom);

					std::stringstream optionStream(line);
					std::string option;

					uint8_t i = 0;
					while (std::getline(optionStream, option, '|'))
					{
						if (i < 1)
						{
							i++;
							continue;
						}

						event.AddOption(option);
						i++;
					}

					events.push_back(event);
				}
				else {
					std::string text = line;

					std::string vowels("aeiou");
					if (vowels.find_first_of(ITEMTYPE_GetItemTypeName(refItem).at(0)) != vowels.size())
					{
						text = std::regex_replace(line, std::regex("a %item%"), "an " + std::string(ITEMTYPE_GetItemTypeName(refItem) + " %item_" + std::to_string(refItem)));
					}

					text = std::regex_replace(line, std::regex("%item%"), std::string(ITEMTYPE_GetItemTypeName(refItem) + " %item_" + std::to_string(refItem)));

					events.push_back(DialogueSequence::DialogueEvent(text, nullptr, zoom));
				}

			}

			DialogueSequence sequence;
			for (DialogueSequence::DialogueEvent de : events)
			{
				sequence.AddDialogueEvent(de);
			}

			return sequence;
		}

#ifdef SHOW_WINDOWS_ERROR_POPUPS
		std::string message = "Could not find dialogue text for: " + sequenceName;
		std::wstring stemp = std::wstring(message.begin(), message.end());
		MessageBox(nullptr, stemp.c_str(), TEXT("Resource Error"), MB_OK);
#endif

#if _DEBUG
		DEBUG::DebugLog::LogError("Failed to find a dialogue sequence \"" + sequenceName + ".\" (DialogueManager.h:87)");
#endif

		return NULL;
	}

	std::string DialogueManager::GetPhrase(std::string sequenceName, ITEM_TYPE refItem)
	{
		if (m_storedDialogueSequences.contains(sequenceName))
		{
			std::string text = m_storedDialogueSequences[sequenceName];

			//If an item was given, replace all instances of %item% with that item.
			if (refItem != ITEM_TYPE::NULL_ITEM)
			{
				std::string vowels("aeiou");
				if (vowels.find_first_of(ITEMTYPE_GetItemTypeName(refItem).at(0)) != vowels.size())
				{
					text = std::regex_replace(text, std::regex("a %item%"), "an " + std::string(ITEMTYPE_GetItemTypeName(refItem) + " %item_" + std::to_string(refItem)));
				}

				text = std::regex_replace(text, std::regex("%item%"), std::string(ITEMTYPE_GetItemTypeName(refItem) + " %item_" + std::to_string(refItem)));
			}

			return m_storedDialogueSequences[sequenceName];
		}

		return sequenceName;
	}

	DialogueSequence DialogueManager::ReplaceDialogueSequenceItem(DialogueSequence& sequence, ITEM_TYPE item)
	{
		DialogueSequence out;
		while (!sequence.IsEmpty())
		{
			DialogueSequence::DialogueEvent event = sequence.Next();
			std::string text = std::regex_replace(event.Text, std::regex("%item%"), std::string(ITEMTYPE_GetItemTypeName(item) + " %item_" + std::to_string(item)));
			
			out.AddDialogueEvent(DialogueSequence::DialogueEvent(text, event.FocusObject, event.Zoom, event.SpeakerAnimation));
		}

		return out;
	}

	void DialogueManager::LoadStoredDialogueSequences(std::string path)
	{
		m_storedDialogueSequences = Resources::AssetManager::GetDialogueData(path.c_str());

		//Load the global dialogue info too.
		for (auto& [key, value] : Resources::AssetManager::GetDialogueData(DIALOGUE_MANAGER_GLOBAL_DIALOGUE_FOLDER))
		{
			m_storedDialogueSequences.emplace(key, value);
		}
	}

	bool DialogueManager::advanceDialogue()
	{
		//Advancing dialogue with a menu open breaks the menu, so don'ten't.
		if (TestGame::GetGamePaused())
		{
			return false;
		}

		if (!m_playingDialogueSequence) { return false; }


		//A textbox does not exist, create it.
		if (std::get<0>(m_guiDisplay) == nullptr) { createDialogueBox(); }

		for (Text::TextRenderer::ExpectedLetter* letter : std::get<1>(m_guiDisplay))
		{
			//The mutex is locked, therefore the text is getting modified so we cannot currently advance dialogue.
			if (letter->letterLock.try_lock())
			{
				letter->letterLock.unlock();
			}
			else
			{
				//Text is still getting animated and modified, don't allow dialogue to advance.
				return false;
			}
		}

		//Remove all existing text.
		for (Text::TextRenderer::ExpectedLetter* letter : std::get<1>(m_guiDisplay))
		{
			Renderer::UnloadGUIElement(letter->letter);

			delete letter->letter;
			delete letter;
		}

		//Advance dialogue sequence.
		m_currentDialogueEvent = m_currentDialogueSequence.Next();

		if (m_currentDialogueEvent.Text.empty())
		{
			//We are at the end of the dialogue.
			stopDialogue();

			return true;
		}

		//Determine if text or options should be rendered.
		if (m_currentDialogueEvent.Options.empty())
		{
			Text::TextRenderer::ExpectedRenderedWord erw = DialogueManager_RenderDialogue(m_currentDialogueEvent.Text);
			m_guiDisplay = std::tuple<StaticGUIElement*, Text::TextRenderer::ExpectedRenderedWord>(std::get<0>(m_guiDisplay), erw);
		}
		else {
			createDialogueOptions();
		}

			//--- Handle camera --//

		updateDialogueEvents();

		return true;
	}

	void DialogueManager::destroyDialogueBox()
	{
		//Cleaning day.

		if (std::get<0>(m_guiDisplay) == nullptr) { return; } //No dialouge, why are you calling this silly.

		Renderer::UnloadGUIElement(std::get<0>(m_guiDisplay));
		delete std::get<0>(m_guiDisplay);

		if (m_guiSpeakingDisplay != nullptr)
		{
			Renderer::UnloadGUIElement(m_guiSpeakingDisplay.get(), 0);
			m_guiSpeakingDisplay.release();
		}

		m_guiDisplay = std::tuple<StaticGUIElement*, Text::TextRenderer::ExpectedRenderedWord>(nullptr, NULL);

	}

	void DialogueManager::createDialogueOptions()
	{
		const std::string& prompt = DialogueManager::m_currentDialogueEvent.Text;
		const auto& optionPaths = DialogueManager::m_currentDialogueEvent.Options;

		//2 buttons present themselves.
		if (optionPaths.size() == 2)
		{
			//Create the first button and text.
			Text::TextRenderer::ExpectedRenderedWord erw = Text::TextRenderer::RenderWordCaseSensitive(prompt.substr(0, prompt.find_first_of('/')), { DIALOGUE_TEXT_BOX_SPACING_X + 12, DIALOGUE_TEXT_BOX_SPACING_Y + 8 }, 9.f, -3, 2, DEFAULT_FONT_RENDER_A_SPRITE_ID, DEFAULT_FONT_RENDER_LOWERCASE_A_SPRITE_ID, std::chrono::milliseconds(62));

			auto s = Renderer::GetSprite(DIALOGUE_TEXT_BOX_TEXTURE);
			GUIButton* button1 = new GUIButton(Vec2{ DIALOGUE_TEXT_BOX_SPACING_X + 2, DIALOGUE_TEXT_BOX_SPACING_Y + 7 }, Vec2{ erw.size() * 6.f + 4.f, 11.f }, s->GetSpriteId(), new std::function<void(int)>(dialogueButtonCallback), 10001, Vec4{1.f, 1.f, 1.f, 1.f}, Vec4{0.5f, 0.5f, 0.f, 1.f});


			//Create the second button and text.
			std::string secondWordStr = prompt.substr(prompt.find_first_of('/') + 1);
			Text::TextRenderer::ExpectedRenderedWord secondWord = Text::TextRenderer::RenderWordCaseSensitive(secondWordStr, { TargetResolutionX - DIALOGUE_TEXT_BOX_SPACING_X - (secondWordStr.length() * 9.f) - 2.f, DIALOGUE_TEXT_BOX_SPACING_Y + 8 }, 9.f, -3, 2, DEFAULT_FONT_RENDER_A_SPRITE_ID, DEFAULT_FONT_RENDER_LOWERCASE_A_SPRITE_ID, std::chrono::milliseconds(62));

			//Merges all text for both buttons into one text object.
			erw.insert(erw.end(), secondWord.begin(), secondWord.end());

			GUIButton* button2 = new GUIButton(Vec2{ secondWord[0]->letter->GetPosition().X - 11.f, DIALOGUE_TEXT_BOX_SPACING_Y + 7 }, Vec2{ secondWord.size() * 6.f + 5.f, 11.f }, s->GetSpriteId(), new std::function<void(int)>(dialogueButtonCallback), 10002, Vec4{1.f, 1.f, 1.f, 1.f}, Vec4{0.5f, 0.5f, 0.f, 1.f});

			//Update the gui display.
			m_guiDisplay = std::tuple<StaticGUIElement*, Text::TextRenderer::ExpectedRenderedWord>(std::get<0>(m_guiDisplay), erw);

			//Load it up:
			Renderer::LoadGUIElement(button1);
			Renderer::LoadGUIElement(button2);

			GUIManager::RegisterButton(button1);
			GUIManager::RegisterButton(button2);

			m_guiOptionButtons[0] = button1;
			m_guiOptionButtons[1] = button2;
		}
	}


	void DialogueManager::updateDialogueEvents()
	{
		if (m_currentDialogueEvent.FocusObject != nullptr)
		{
			static_cast<GAME_NAME::Camera::GameCamera*>(TestGame::INSTANCE->GetCamera())->LockCamera(m_currentDialogueEvent.FocusObject);

			
			Objects::Player::Player* player = dynamic_cast<Objects::Player::Player*>(m_currentDialogueEvent.FocusObject);
			if (player)
			{
				if (m_currentDialogueEvent.SpeakerAnimation != AnimationState::NO_LOOK_DIRECTION)
				{
					player->SetFrozen(false);
					player->SetFrozen(true, m_currentDialogueEvent.SpeakerAnimation);
				}
			}

			AnimatingCharacter* character = dynamic_cast<AnimatingCharacter*>(m_currentDialogueEvent.FocusObject);
			if (character)
			{
				if (m_currentDialogueEvent.SpeakerAnimation != AnimationState::NO_LOOK_DIRECTION)
				{
					if (character->IsFrozen())
					{
						character->SetFrozen(false);
					}

					character->SetFrozen(true, m_currentDialogueEvent.SpeakerAnimation);
				}
			}

			updateSpeakerDisplay();
		}

		if (m_currentDialogueEvent.Zoom > 0)
		{
			static_cast<GAME_NAME::Camera::GameCamera*>(TestGame::INSTANCE->GetCamera())->SetTargetZoom(m_currentDialogueEvent.Zoom);
		}
	}

#define SPEAKING_DISPLAY_VERTICAL_PORTION 0.6f

	const Vec2 DialogueManager_SpeakingSpriteVerts[4] = {
		Vec2(1.0f, SPEAKING_DISPLAY_VERTICAL_PORTION),
		Vec2(1.0f, 0.f),
		Vec2(0.0f, 0.f),
		Vec2(0.0f, SPEAKING_DISPLAY_VERTICAL_PORTION)
	};

#define SPEAKING_DISPLAY_POSITION Vec2{ 4.5f, 0.f }
#define SPEAKING_DISPLAY_SCALE_SCALAR 3.f

	void DialogueManager::updateSpeakerDisplay()
	{
		//No focus object, no speaker.
		if (m_currentDialogueEvent.FocusObject == nullptr) { return; }

		unsigned int speakingSpriteId(m_currentDialogueEvent.FocusObject->GetSprite()->GetSpriteId());

		//If the display has the same id, no change occured with the sprte.
		if (m_guiSpeakingDisplay != nullptr && m_guiSpeakingDisplay->GetSprite()->GetSpriteId() == speakingSpriteId)
		{
			m_guiSpeakingDisplayOpacity = std::lerp(m_guiSpeakingDisplayOpacity, 1.f, Utils::Time::GameTime::GetScaledDeltaTime() * 1.75f);

			Vec4 colors[4] = {
				Vec4{ 1.f, 1.f, 1.f, m_guiSpeakingDisplayOpacity },
				Vec4{ 1.f, 1.f, 1.f, m_guiSpeakingDisplayOpacity },
				Vec4{ 1.f, 1.f, 1.f, m_guiSpeakingDisplayOpacity },
				Vec4{ 1.f, 1.f, 1.f, m_guiSpeakingDisplayOpacity }
			};

			dynamic_cast<DynamicSprite*>(m_guiSpeakingDisplay->GetSprite().get())->UpdateTextureColor(colors);

			return;
		}

		//Make the display transparent as it is changing now.
		m_guiSpeakingDisplayOpacity = 0.f;

		Vec2 speakingSpriteScale(m_currentDialogueEvent.FocusObject->GetScale());

		//Create the character speaking display GUI if the current focus object is a character or the player.
		DynamicSprite* dialogueSprite = new DynamicSprite(speakingSpriteId);
		dialogueSprite->UpdateTextureVerticies(DialogueManager_SpeakingSpriteVerts);

		//Position the speaking sprite above the left side of the text box.
		if (m_guiSpeakingDisplay == nullptr)
		{
			m_guiSpeakingDisplay = std::make_unique<StaticGUIElement>(std::get<0>(m_guiDisplay)->GetPosition() + Vec2{ 0.f, std::get<0>(m_guiDisplay)->GetScale().Y } + SPEAKING_DISPLAY_POSITION, Vec2{ 1.f, 1.f }, nullptr);
			Renderer::LoadGUIElement(m_guiSpeakingDisplay.get(), 0);
		}

		m_guiSpeakingDisplay->SetSprite(dialogueSprite);
		m_guiSpeakingDisplay->SetScale(speakingSpriteScale * Vec2{ SPEAKING_DISPLAY_SCALE_SCALAR, SPEAKING_DISPLAY_SCALE_SCALAR * SPEAKING_DISPLAY_VERTICAL_PORTION });
	}

	void DialogueManager::dialogueButtonCallback(int buttonId)
	{
		//There are no options???? This shouldn't happen ever... assert()
		if (DialogueManager::m_currentDialogueEvent.Options.size() < 1)
		{
			return;
		}

		//Ensure that text is done getting modified by other threads. (After text is unlocked it is never locked again, so maintaining ownership of the mutex is not necessary.)
		for (Text::TextRenderer::ExpectedLetter* letter : std::get<1>(DialogueManager::INSTANCE->m_guiDisplay))
		{
			//The mutex is locked, therefore the text is getting modified so we cannot currently advance dialogue.
			if (letter->letterLock.try_lock())
			{
				letter->letterLock.unlock();
			}
			else
			{
				return;
			}
		}

		//Remove all the text.
		for (Text::TextRenderer::ExpectedLetter* letter : std::get<1>(DialogueManager::INSTANCE->m_guiDisplay))
		{
			Renderer::UnloadGUIElement(letter->letter, 2);

			delete letter->letter;
			delete letter;
		}

		//Remove everything else.
		DialogueManager::INSTANCE->stopDialogue();
		
		//Ensure the GUI option buttons are removed and deleted, as they are no longer needed.
		for (int i = 0; i < m_guiOptionButtons.size(); i++)
		{
			if (m_guiOptionButtons[i] != nullptr)
			{
				GUIManager::UnregisterButton(m_guiOptionButtons[i]);
				Renderer::UnloadGUIElement(m_guiOptionButtons[i]);

				delete m_guiOptionButtons[i];
			}
		}

		//Determine what the next dialogue will be.

		int path = buttonId - 10001;

		if (DialogueManager::m_currentDialogueEvent.Options.size() > path)
		{
			DialogueManager::INSTANCE->PlayDialogueSequence(*DialogueManager::m_currentDialogueEvent.Options.at(path));
		}
	}
}
