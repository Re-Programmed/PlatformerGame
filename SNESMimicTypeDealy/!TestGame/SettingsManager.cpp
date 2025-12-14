#include "SettingsManager.h"
#include "../Input/InputManager.h"

#include "../Objects/GUI/GUIButton.h"
#include "../Objects/GUI/GUIManager.h"

#include "./TestGame.h"

#include <algorithm>

#include "../Utils/Time/GameTime.h"

#include "../Settings/SettingsGlobals.h"

#include "../Rendering/Window.h"

//The amount that a GLFW_KEY should be offset to become a displayable char.
#define LETTER_DISPLAY_OFFSET ('a' - GLFW_KEY_A)

namespace GAME_NAME
{
	double SettingsManager_buttonCooldown;

	std::vector<StaticGUIElement*> SettingsManager::m_currentPageElements;

	std::vector<StaticGUIElement*> SettingsManager::m_controlsPageNumbers;

	std::vector<StaticGUIElement*> SettingsManager::m_recordingText;

	std::vector<StaticGUIElement*> SettingsManager::m_musicMixerPercentage, SettingsManager::m_sfxMixerPercentage;

	bool SettingsManager::m_settingsMenuOpen = false;

	bool SettingsManager::m_recordingKey = false;
	int SettingsManager::m_currentRecordingIndex = -1;
	
	std::vector<StaticGUIElement*> ;

	void SettingsManager::CreateSettingsMenu()
	{
		m_settingsMenuOpen = true;

		Renderer::UpdateObjects = false;

		GUI::Menus::GUIMenu::LoadMenu("/settings", new std::function(mainMenu_guiCallback));
	}

//Dims the screen by creating a large, dark rectangle.
#define CreateMenuBacking() GUI::StaticGUIElement* backing = new GUI::StaticGUIElement({ 0.f, -800.f }, { 400.f, 1000.f }, Renderer::GetSprite(SpriteBase(-1))->GetSpriteId()); Renderer::LoadGUIElement(backing, 1); m_currentPageElements.push_back(backing);

	double SettingsManager_currentControlsMenuScroll = 0.0; //Used to determine where the top and bottom of the scrolling page should be.
	StaticGUIElement* SettingsManager_applyButton = nullptr;


	void SettingsManager::OpenSoundMenu()
	{
		SettingsManager_buttonCooldown = 1.0;

		m_settingsMenuOpen = true;
		Renderer::UpdateObjects = false;

		CreateMenuBacking();

		Text::TextRenderer::RenderedWord controlsName = Text::TextRenderer::RenderWord("sound", Vec2(128.33f, 161), 12.f, 0.f, 2);
		m_currentPageElements.insert(m_currentPageElements.end(), controlsName.begin(), controlsName.end());

using namespace GUI::Text;

		GUI::GUIButton* applyButton = new GUI::GUIButton(Vec2(78.6f, 118.f), Vec2(72.f, 16.f), Renderer::GetSprite(72)->GetSpriteId(), new std::function(soundMenu_guiCallback), Vec4{ 0.f, 0.75f, 0.f, 1.f });
		Renderer::LoadGUIElement(applyButton, 2);
		SettingsManager_applyButton = applyButton;
		m_currentPageElements.push_back(applyButton);
		GUI::GUIManager::RegisterButton(applyButton);

		GUI::GUIButton* closeButton = new GUI::GUIButton(Vec2(162.f, 118.f), Vec2(72.f, 16.f), Renderer::GetSprite(72)->GetSpriteId(), new std::function(soundMenu_guiCallback), Vec4{ 0.75f, 0.f, 0.f, 1.f });
		Renderer::LoadGUIElement(closeButton, 2);
		m_currentPageElements.push_back(closeButton);
		GUI::GUIManager::RegisterButton(closeButton);

		auto applyWords = TextRenderer::RenderWord("Apply", Vec2(96.6f, 120.f), 12.f, 0.f, 2);
		m_currentPageElements.insert(m_currentPageElements.end(), applyWords.begin(), applyWords.end());

		auto closeWords = TextRenderer::RenderWord("Close", Vec2(180.f, 120.f), 12.f, 0.f, 2);
		m_currentPageElements.insert(m_currentPageElements.end(), closeWords.begin(), closeWords.end());


		//MUSIC MIXER (id_2)
		GUI::GUIButton* musicMixerKnob = new GUI::GUIButton({ 78.6f, 15.f }, { 12.2f, 6.11f }, Renderer::GetSprite(72)->GetSpriteId(), new std::function(soundMenu_guiCallback), Vec4{ 1.f, 1.f, 1.f, 1.f });
		Renderer::LoadGUIElement(musicMixerKnob, 2);
		m_currentPageElements.push_back(musicMixerKnob);
		GUI::GUIManager::RegisterButton(musicMixerKnob);

		GUI::StaticGUIElement* musicMixerBGLine = new GUI::StaticGUIElement(musicMixerKnob->GetPosition() + Vec2{musicMixerKnob->GetScale().X / 2.f - 0.5f, 0.f}, {1.f, 100.f}, Renderer::GetSprite(72)->GetSpriteId());
		Renderer::LoadGUIElement(musicMixerBGLine, 2);
		m_currentPageElements.push_back(musicMixerBGLine);

		float percentage = Audio::SoundManager::MusicBus.mVolume;

		{
			//Calculate where the knob should display based on the existing volume.
			float yPosAdd = percentage * (musicMixerBGLine->GetScale().Y - musicMixerKnob->GetScale().Y / 2.f) - musicMixerKnob->GetScale().Y / 2.f;
			musicMixerKnob->SetPosition({ musicMixerKnob->GetPosition().X, yPosAdd + musicMixerKnob->GetPosition().Y });
		}


		auto musicMixerHeader = TextRenderer::RenderWord("Music", Vec2{ musicMixerKnob->GetPosition().X + musicMixerKnob->GetScale().X + 6.f, musicMixerBGLine->GetPosition().Y + musicMixerBGLine->GetScale().Y - 6.f }, 6.f, 0.f, 2);
		m_currentPageElements.insert(m_currentPageElements.end(), musicMixerHeader.begin(), musicMixerHeader.end());

		Vec2 musicMixerPercentagePosition = musicMixerHeader.at(0)->GetPosition() - Vec2{ 0.f, 6.75f };
		m_musicMixerPercentage = TextRenderer::RenderNumber(static_cast<int>(percentage * 100), musicMixerPercentagePosition, 0.25f, 0.f, 3);

		//SFX MIXER (id_3)
		GUI::GUIButton* sfxMixerKnob = new GUI::GUIButton({ 108.6f, 15.f }, { 12.2f, 6.11f }, Renderer::GetSprite(72)->GetSpriteId(), new std::function(soundMenu_guiCallback), Vec4{ 1.f, 1.f, 1.f, 1.f });
		Renderer::LoadGUIElement(sfxMixerKnob, 2);
		m_currentPageElements.push_back(sfxMixerKnob);
		GUI::GUIManager::RegisterButton(sfxMixerKnob);

		GUI::StaticGUIElement* sfxMixerBGLine = new GUI::StaticGUIElement(sfxMixerKnob->GetPosition() + Vec2{ sfxMixerKnob->GetScale().X / 2.f - 0.5f, 0.f }, { 1.f, 100.f }, Renderer::GetSprite(72)->GetSpriteId());
		Renderer::LoadGUIElement(sfxMixerBGLine, 2);
		m_currentPageElements.push_back(sfxMixerBGLine);

		percentage = Audio::SoundManager::SFXBus.mVolume;

		{
			//Calculate where the knob should display based on the existing volume.
			float yPosAdd = percentage * (sfxMixerBGLine->GetScale().Y - sfxMixerKnob->GetScale().Y / 2.f) - sfxMixerKnob->GetScale().Y / 2.f;
			sfxMixerKnob->SetPosition({ sfxMixerKnob->GetPosition().X, yPosAdd + sfxMixerKnob->GetPosition().Y });
		}


		auto sfxMixerHeader = TextRenderer::RenderWord("SFX", Vec2{ sfxMixerKnob->GetPosition().X + sfxMixerKnob->GetScale().X + 8.25f, sfxMixerBGLine->GetPosition().Y + sfxMixerBGLine->GetScale().Y - 6.f }, 6.f, 0.f, 2);
		m_currentPageElements.insert(m_currentPageElements.end(), sfxMixerHeader.begin(), sfxMixerHeader.end());

		Vec2 sfxMixerPercentagePosition = sfxMixerHeader.at(0)->GetPosition() - Vec2{ 0.f, 6.75f };
		m_sfxMixerPercentage = TextRenderer::RenderNumber(static_cast<int>(percentage * 100), sfxMixerPercentagePosition, 0.25f, 0.f, 3);


		m_currentRecordingIndex = -1;
	}

	void SettingsManager::OpenGraphicsMenu()
	{
		CreateMenuBacking();

		m_settingsMenuOpen = true;
		Renderer::UpdateObjects = false;

using namespace GUI::Text;

		Sprite* sp = Renderer::GetSprite(72);


		GUI::GUIButton* closeButton = new GUI::GUIButton(Vec2(TargetResolutionX/2.f - 48.f, 138.f), Vec2(72.f, 16.f), sp->GetSpriteId(), new std::function(soundMenu_guiCallback), Vec4{ 0.75f, 0.f, 0.f, 1.f });
		Renderer::LoadGUIElement(closeButton, 2);
		m_currentPageElements.push_back(closeButton);
		GUI::GUIManager::RegisterButton(closeButton);

		auto closeWords = TextRenderer::RenderWord("Close", Vec2(TargetResolutionX / 2.f - 28.f, 140.f), 12.f, 0.f, 2);
		m_currentPageElements.insert(m_currentPageElements.end(), closeWords.begin(), closeWords.end());


		bool& isFullscreen = AppData::Settings::SettingsGlobals::Fullscreen.Value;

		TextRenderer::RenderedWord fullscreenOptionText = GUI::Text::TextRenderer::RenderWord("Fullscreen: ", Vec2{ 24.f, 118.f }, 12.f, 0.f, 2);
		m_currentPageElements.insert(m_currentPageElements.end(), fullscreenOptionText.begin(), fullscreenOptionText.end());

		GUIButton* fullscreenOptionButton = new GUIButton(Vec2{ 162.f, 116.f }, Vec2{ 16.f, 16.f }, sp->GetSpriteId(), new std::function(graphicsMenu_guiCallback), isFullscreen ? Vec4{ 0.f, 1.f, 0.f, 1.f } : Vec4{ 1.f, 0.f, 0.f, 1.f }, Vec4{ 0.5f, 0.5f, 0.f, 1.f });
		Renderer::LoadGUIElement(fullscreenOptionButton, 2);
		m_currentPageElements.push_back(fullscreenOptionButton);
		GUIManager::RegisterButton(fullscreenOptionButton);

		delete sp;
	}




	void SettingsManager::OpenControlsMenu()
	{
		//TODO: This cannot update the scroll function because it overrides other processes scroll callbacks.
		// USE THE INPUT MANAGER SYSTEM.
		glfwSetScrollCallback(TestGame::FirstWindow, controlsMenu_scrollCallback);

		m_settingsMenuOpen = true;
		
		Renderer::UpdateObjects = false;

		SettingsManager_currentControlsMenuScroll = 0;

		//GUI::Menus::GUIMenu::LoadMenu("/settings_controls", new std::function(controlsMenu_guiCallback));

		CreateMenuBacking();

		Text::TextRenderer::RenderedWord controlsName = Text::TextRenderer::RenderWord("controls", Vec2(118, 161), 12.f, 0.f, 2);
		m_currentPageElements.insert(m_currentPageElements.end(), controlsName.begin(), controlsName.end());

using namespace GUI::Text;

		GUI::GUIButton* applyButton = new GUI::GUIButton(Vec2(78.6f, 118.f), Vec2(72.f, 16.f), Renderer::GetSprite(72)->GetSpriteId(), new std::function(controlsMenu_guiCallback), Vec4{ 0.f, 0.75f, 0.f, 1.f });
		Renderer::LoadGUIElement(applyButton, 2);
		SettingsManager_applyButton = applyButton;
		m_currentPageElements.push_back(applyButton);
		SettingsManager_currentControlsMenuScroll = SettingsManager_applyButton->GetPosition().Y;
		GUI::GUIManager::RegisterButton(applyButton);

		GUI::GUIButton* closeButton = new GUI::GUIButton(Vec2(162.f, 118.f), Vec2(72.f, 16.f), Renderer::GetSprite(72)->GetSpriteId(), new std::function(controlsMenu_guiCallback), Vec4{ 0.75f, 0.f, 0.f, 1.f });
		Renderer::LoadGUIElement(closeButton, 2);
		m_currentPageElements.push_back(closeButton);
		GUI::GUIManager::RegisterButton(closeButton);

		auto applyWords = TextRenderer::RenderWord("Apply", Vec2(96.6f, 120.f), 12.f, 0.f, 2);
		m_currentPageElements.insert(m_currentPageElements.end(), applyWords.begin(), applyWords.end());

		auto closeWords = TextRenderer::RenderWord("Close", Vec2(180.f, 120.f), 12.f, 0.f, 2);
		m_currentPageElements.insert(m_currentPageElements.end(), closeWords.begin(), closeWords.end());

		for (int i = 0; i < KEY_ARRAY_SIZE; i++)
		{
			std::string name = KeybindNames[i].data();
			std::replace(name.begin(), name.end(), '_', ' ');
			TextRenderer::RenderedWord keyName = TextRenderer::RenderWord(name, Vec2(24, 102.f - (i * 16.f)), 8.f, 0.f, 2);
			m_currentPageElements.insert(m_currentPageElements.begin(), keyName.begin(), keyName.end());

			Vec2 keyDisplayPosition = Vec2(250, 102.f - (i * 16.f));

			GUI::GUIButton* keyInput = new GUI::GUIButton(keyDisplayPosition, Vec2(12.f, 12.f), Renderer::GetSprite(72)->GetSpriteId(), new std::function(controlsMenu_guiCallback), Vec4{ 0.6f, 0.2f, 0.4f, 1.f }, Vec4{ 1.f, 1.f, 0.f, 1.f });
			GUI::GUIManager::RegisterButton(keyInput);
			Renderer::LoadGUIElement(keyInput);
			m_currentPageElements.push_back(keyInput);

			keyDisplayPosition.X += 12.f;

			if (InputManager::GetKeybindValue(i) <= GLFW_KEY_9 && InputManager::GetKeybindValue(i) >= GLFW_KEY_0)
			{
				Vec2 pos = (keyDisplayPosition + Vec2(-1.4f, 1.f));
				StaticGUIElement* digit = TextRenderer::RenderDigit(InputManager::GetKeybindValue(i) - GLFW_KEY_0, pos, 12.f / 32.f);
				m_currentPageElements.push_back(digit);
				m_controlsPageNumbers.push_back(digit);
			}
			else {
				TextRenderer::Letter keyLetterDisplay = TextRenderer::RenderLetter(InputManager::GetKeybindValue(i) + LETTER_DISPLAY_OFFSET, keyDisplayPosition, 12.f, 2);
				m_controlsPageNumbers.push_back(keyLetterDisplay);
				m_currentPageElements.push_back(keyLetterDisplay);
			}
		}
	}

	void SettingsManager::mainMenu_guiCallback(int id)
	{
		std::cout << "ID OF BUTTON: " << id << std::endl; //TESTING BUTTONS.

		switch (id)
		{
		//Controls button.
		case 0:
			CloseMenu();
			OpenControlsMenu();
			return;
		//Sound button.
		case 1:
			CloseMenu();
			OpenSoundMenu();
			return;
		//Graphics button.
		case 2:
			CloseMenu();
			OpenGraphicsMenu();
			return;
		//Back button.
		case 3:
			CloseMenu();
			return;
		}
	}



	void SettingsManager::controlsMenu_guiCallback(int id)
	{
		if (m_recordingKey) { return; }

		if (id == 0)
		{
			//Apply button.
			InputManager::SaveAllKeybinds();
		}

		if (id <= 1)
		{
			m_controlsPageNumbers.clear();

			/*
				SHOULD HAVE USED SMART POUINTER HERE STUFF IS GETTING DELETEED AND ASAASPDFJSAKFJSAKLF
				TODO: USE SMART POINTER STUFF PLEAES
			*/
			for (GUI::StaticGUIElement* el : m_currentPageElements)
			{
				if (el == nullptr) { continue; }

				Renderer::UnloadGUIElement(el, 1);
				Renderer::UnloadGUIElement(el, 2);

				/*
					BUG WITH THE FACT TAHT UPDATING A KEYBIND CAUSES A NULL OBJECT TO EXIST IN THE m_currentPageElements ARRAY.
				*/

				try {
					if (GUI::GUIButton* button = dynamic_cast<GUI::GUIButton*>(el))
					{
						GUIManager::UnregisterButton(button);
					}
				}
				catch (...)
				{

				}

				delete el;
			}

			m_currentPageElements.clear();
			
			CloseMenu(true);

			return;
		}

		m_currentRecordingIndex = id - 2;

		std::cout << "ID OF BUTTON: " << id << std::endl; //TESTING BUTTONS.

		m_recordingKey = true;

		m_recordingText = Text::TextRenderer::RenderWord("Recording", Vec2(100.f, 40.f), 16.f, 0.f, 2);

		//Record keys to detect what the player is binding.
		std::thread recordThread([]() {

			while (true)
			{
				if (m_currentRecordingIndex < 0) { break; }
				if (!m_recordingKey) { break; }

				for (int i = 0; i < m_recordingText.size(); i++)
				{
					m_recordingText[i]->SetScale({ (2.f + (float)std::sin(glfwGetTime())) * -16.f/3.f, (2.f + (float)std::sin(glfwGetTime())) * 16.f/3.f });
				}

				//SOMEHOW DISPLAY WHAT KEY IS GETTING RECORDED TO.
				//m_controlsPageNumbers[m_currentRecordingIndex]->SetScale({ -20.f/4.f * (2.f + (float)std::sin(glfwGetTime() * 2.0)), 12.f });

				/*
					FIX TO WORK WITH ALL KEYS, NOT JUST LETTERS.
				*/

				for (int i = 48; i <= 90; i++)
				{
					if (glfwGetKey(TestGame::FirstWindow, i) == GLFW_PRESS)
					{
						//Update keybind graphic.
						m_recordingKey = false;
						StaticGUIElement* const lastElement = m_controlsPageNumbers[m_currentRecordingIndex];

						Renderer::UnloadGUIElement(m_controlsPageNumbers[m_currentRecordingIndex], 2);
						m_controlsPageNumbers[m_currentRecordingIndex] = Text::TextRenderer::RenderLetter(i + LETTER_DISPLAY_OFFSET, lastElement->GetPosition(), lastElement->GetScale().Y, 2);
						m_currentPageElements.push_back(m_controlsPageNumbers[m_currentRecordingIndex]);

						//Remove from the current page list.
						int a = 0;
						for (StaticGUIElement*& el : m_currentPageElements)
						{
							if (el == lastElement)
							{
								m_currentPageElements.erase(m_currentPageElements.begin() + a);
								break;
							}

							a++;
						}

						delete lastElement;

						for (int i = 0; i < m_recordingText.size(); i++)
						{
							Renderer::UnloadGUIElement(m_recordingText[i], 2);
							delete m_recordingText[i];
						}

						m_recordingText.clear();

						//Update keybind data.
						InputManager::UpdateKeybind((keyRef)m_currentRecordingIndex, i);
						break;
					}
				}
			}
		});

		recordThread.detach();
	}

	void SettingsManager::soundMenu_guiCallback(int id)
	{
		if (SettingsManager_buttonCooldown > 0) { return; }

		if (id == 0)
		{
			//Apply button.
			Audio::SoundManager::SaveAllAudioBusVolumes();
		}

		if (id <= 1)
		{

			for (GUI::StaticGUIElement* el : m_currentPageElements)
			{
				if (el == nullptr) { continue; }

				Renderer::UnloadGUIElement(el, 1);
				Renderer::UnloadGUIElement(el, 2);

				/*
					BUG WITH THE FACT TAHT UPDATING A KEYBIND CAUSES A NULL OBJECT TO EXIST IN THE m_currentPageElements ARRAY.
				*/

				try {
					if (GUI::GUIButton* button = dynamic_cast<GUI::GUIButton*>(el))
					{
						GUIManager::UnregisterButton(button);
					}
				}
				catch (...)
				{

				}

				delete el;
			}

			for (GUI::StaticGUIElement* el : m_musicMixerPercentage)
			{
				Renderer::UnloadGUIElement(el);

				delete el;
			}

			for (GUI::StaticGUIElement* el : m_sfxMixerPercentage)
			{
				Renderer::UnloadGUIElement(el);

				delete el;
			}

			m_musicMixerPercentage.clear();
			m_sfxMixerPercentage.clear();

			m_currentPageElements.clear();

			CloseMenu(true);

			return;
		}


		if (m_currentRecordingIndex < 0)
		{
			//MUSIC Mixer Knob
			if (id == 2)
			{
				m_currentRecordingIndex = id;
			}

			//SFX Mixer Knob
			if (id == 3)
			{
				m_currentRecordingIndex = id;
			}
		}


	}

	void SettingsManager::graphicsMenu_guiCallback(int id)
	{
		if (SettingsManager_buttonCooldown > 0.0)
		{
			return;
		}


		if (id <= 0)
		{
			for (GUI::StaticGUIElement* el : m_currentPageElements)
			{
				if (el == nullptr) { continue; }

				Renderer::UnloadGUIElement(el, 1);
				Renderer::UnloadGUIElement(el, 2);

				try {
					if (GUI::GUIButton* button = dynamic_cast<GUI::GUIButton*>(el))
					{
						GUIManager::UnregisterButton(button);
					}
				}
				catch (...)
				{

				}

				delete el;
			}
		}

		//Fullscreen toggle.
		if (id == 1)
		{
			Window::INSTANCE->SetFullscreen(!Window::INSTANCE->GetFullscreen());
			SettingsManager_buttonCooldown = 1.0;
		}


		
	}

	GUI::Text::TextRenderer::RenderedDigit SettingsManager::UpdateAudioMixer(int mixerIndex, int& percentDisplay, GUI::Text::TextRenderer::RenderedDigit& const initialDigit)
	{
		GUI::StaticGUIElement* knob = m_currentPageElements[18 /*Elements before the first mixer knob*/ + (m_currentRecordingIndex - mixerIndex /*Buttons before the first mixer knob*/)];
		knob->SetPosition(Vec2{ knob->GetPosition().X, InputManager::GetMouseScreenPosition().Y - knob->GetScale().Y / 2.f });

		GUI::StaticGUIElement* knobTrack = (m_currentPageElements[18 + (m_currentRecordingIndex - mixerIndex) + 1 /*Next element is bg element*/]);

		double percentage = (knob->GetPosition().Y - knobTrack->GetPosition().Y) / (knobTrack->GetScale().Y - knob->GetScale().Y);
		percentDisplay = (int)(percentage * 100);

		if (percentDisplay < 0)
		{
			percentDisplay = 0;
			knob->SetPosition(Vec2{ knob->GetPosition().X, knobTrack->GetPosition().Y });
		}
		else if (percentDisplay > 100) {
			percentDisplay = 100;
			knob->SetPosition(Vec2{ knob->GetPosition().X, knobTrack->GetPosition().Y + knobTrack->GetScale().Y - knob->GetScale().Y });
		}

		Vec2 musicMixerPercentagePosition = initialDigit.at(0)->GetPosition() - Vec2(10.f, 0.f);
		for (GUI::StaticGUIElement* el : initialDigit)
		{
			Renderer::UnloadGUIElement(el);

			delete el;
		}

		return GUI::Text::TextRenderer::RenderNumber(percentDisplay, musicMixerPercentagePosition, 0.25f, 0.f, 3);

	}
	
	void SettingsManager::Update()
	{

		if (!m_settingsMenuOpen) { return; }

		if (SettingsManager_buttonCooldown > 0) {
			SettingsManager_buttonCooldown -= Utils::Time::GameTime::DeltaTime::GetDeltaTime();
		}

		//Records audio knob position.
		if (m_currentRecordingIndex >= 0)
		{
			if (InputManager::GetMouseButton(0))
			{

				//Update the actual volume of the group based on which knob is pressed.
				switch (m_currentRecordingIndex)
				{
					//MUSIC group knob.
				case (2):
				{
					int percentDisplay = 0;
					m_musicMixerPercentage = UpdateAudioMixer(2, percentDisplay, m_musicMixerPercentage);
					Audio::SoundManager::MusicBus.setVolume(static_cast<float>(percentDisplay) / 100.f);
					break;
				}

					//SFX group knob.
				case (3):
				{
					int percentDisplay = 0;
					m_sfxMixerPercentage = UpdateAudioMixer(-4, percentDisplay, m_sfxMixerPercentage);
					Audio::SoundManager::SFXBus.setVolume(static_cast<float>(percentDisplay) / 100.f);
					break;
				}

				default:
					//We clicked something else I guess.
					break;
				}


			}
			else
			{
				m_currentRecordingIndex = -1;
			}
		}
	}

	void SettingsManager::controlsMenu_scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
	{
		//Invert & increase scroll sensitivity.
		yOffset *= -4.0;


		//Prevent scrolling up more than the apply button.
		if (SettingsManager_applyButton->GetPosition().Y < SettingsManager_currentControlsMenuScroll && yOffset < 0)
		{
			return;
		}
		
		for (GUI::StaticGUIElement* element : m_currentPageElements)
		{
			if (element == nullptr) { continue; }
			element->SetPosition({ element->GetPosition().X, element->GetPosition().Y + (float)yOffset });
		}
	}
}
