#include "DisplayIconManager.h"

#include "../../!TestGame/TestGame.h"

namespace GAME_NAME::Input
{
	constexpr int ALetterTexture = SpriteBase(75);

	std::unordered_map<keyRef, DisplayIconManager::KeyDisplay> DisplayIconManager::m_keyDisplays;
	const int DisplayIconManager::m_keyDisplayBaseTexture = SpriteBase(44);

	void DisplayIconManager::ShowKeyInputDisplay(keyRef key, Vec2 anchor, char progress, std::string optionDescription)
	{
		Rendering::Sprite* texture = nullptr;

		//Check if we are using a controller. If we are, the input for the controller should be shown rather than the keyboard.
		if (!InputManager::GetUsingMouse())
		{
			std::vector<InputManager::ControllerInput>* controllerInputs = InputManager::GetControllerInputForAction(key);
			if (controllerInputs != nullptr)
			{
				texture = getTextureForControllerButton((*controllerInputs)[0].Ref);
			}
		}
		else {
			texture = getTextureForKey(key);
		}

		if (m_keyDisplays.contains(key))
		{
			//-------Update the display-------

			//Move each consecutive input display down below the other ones.
			anchor += Vec2{ 2.f, -7.f } * (float)m_keyDisplays[key].Index;

			//Used to track if the display object should be deleted or if it's still in use.
			m_keyDisplays[key].WasDrawnOnLastFrame = true;

			m_keyDisplays[key].BGDisplayObject->SetSprite(Renderer::GetSprite(m_keyDisplayBaseTexture + progress));

			//Used to move all the letters in the descriptor text accordingly.
			Vec2 initialBGPos = m_keyDisplays[key].BGDisplayObject->GetPosition();

			m_keyDisplays[key].BGDisplayObject->SetPosition(anchor);
			m_keyDisplays[key].TextDisplayObject->SetPosition(anchor + Vec2{ -1.f, 1.f });
			int i = 0;
			for (Text::TextRenderer::ExpectedLetter* letter : m_keyDisplays[key].OptionEventDescription)
			{
				letter->letterLock.lock();

				letter->letter->SetPosition(TestGame::INSTANCE->GetCamera()->GlobalToUI(anchor + Vec2{ 10.f + (i++ * 3.f), 0.f}));
				letter->letterLock.unlock();
			}

			if (m_keyDisplays[key].OptionEventDescriptionBG != nullptr)
			{
				m_keyDisplays[key].OptionEventDescriptionBG->SetPosition(m_keyDisplays[key].OptionEventDescription[0]->letter->GetPosition() - Vec2{ 5.5f, 0.5f });
			}

			return;
		}

		//-------Create the display-------
		
		GameObject* keyDisplayObject = new GameObject(anchor, Vec2(-6, 6), Renderer::GetSprite(m_keyDisplayBaseTexture + progress), 0.0f);
		Renderer::LoadActiveObject(keyDisplayObject, 2);

		GameObject* textDisplayObject = new GameObject(anchor + Vec2{ -1.f, 1.f }, Vec2(-4, 4), texture, 0.f);
		Renderer::LoadActiveObject(textDisplayObject, 2);

		Text::TextRenderer::ExpectedRenderedWord descriptorWord;
		GUI::StaticGUIElement* descriptorWordBG = nullptr;

		if (!optionDescription.empty())
		{
			descriptorWord = Text::TextRenderer::RenderWordCaseSensitive(optionDescription, TestGame::INSTANCE->GetCamera()->GlobalToUI(anchor + Vec2{ 10.f, 0.f }), 5.f, -1.f, 1);

			//Correctly position the black square behind the text for what this button input will do.
			float endWordPosition = descriptorWord[descriptorWord.size() - 1]->letter->GetPosition().X + 3.f;
			descriptorWordBG = new StaticGUIElement(descriptorWord[0]->letter->GetPosition() - Vec2{ 5.5f, 0.5f }, Vec2{ endWordPosition - descriptorWord[0]->letter->GetPosition().X + 1.f, 6.f}, Renderer::GetSprite(SpriteBase(-1)));
			Renderer::LoadGUIElement(descriptorWordBG, 0);
		}

		m_keyDisplays.emplace(key, KeyDisplay(m_keyDisplays.size(), keyDisplayObject, textDisplayObject, descriptorWord, descriptorWordBG));
	}

	void DisplayIconManager::AttemptHideIcons()
	{
		for (auto& [key, keyDisplay] : m_keyDisplays)
		{
			//If we are currently using it, don't delete it.
			if(keyDisplay.WasDrawnOnLastFrame){
				keyDisplay.WasDrawnOnLastFrame = false;
				continue;
			}

			Renderer::DestroyActiveObjectImmediate(keyDisplay.BGDisplayObject);
			Renderer::DestroyActiveObjectImmediate(keyDisplay.TextDisplayObject);
			if (keyDisplay.OptionEventDescriptionBG != nullptr)
			{
				Renderer::UnloadGUIElement(keyDisplay.OptionEventDescriptionBG, 0);
			}

			if (keyDisplay.OptionEventDescription.size() > 0)
			{
				for (Text::TextRenderer::ExpectedLetter* letter : keyDisplay.OptionEventDescription)
				{
					letter->letterLock.lock();
					Renderer::UnloadGUIElement(letter->letter);
					letter->letterLock.unlock();
				}
			}

			m_keyDisplays.erase(key);
			break;
		}
	}


	Rendering::Sprite* DisplayIconManager::getTextureForControllerButton(controllerRef controllerButton)
	{
		int keyFound = 0;
		switch (controllerButton)
		{
		case controllerRef::BUTTON_A:
			keyFound = GLFW_KEY_A;
			break;
		case controllerRef::BUTTON_B:
			keyFound = GLFW_KEY_B;
			break;
		case controllerRef::BUTTON_X:
			keyFound = GLFW_KEY_X;
			break;
		case controllerRef::BUTTON_Y:
			keyFound = GLFW_KEY_Y;
			break;
		case controllerRef::BUTTON_R:
		case controllerRef::BUTTON_ZR:
			keyFound = GLFW_KEY_R;
			break;
		case controllerRef::BUTTON_L:
		case controllerRef::BUTTON_ZL:
			keyFound = GLFW_KEY_L;
			break;
		case controllerRef::JOY_1_AXIS_X_NEGATIVE:
		case controllerRef::JOY_1_AXIS_X_POSITIVE:
		case controllerRef::JOY_1_AXIS_Y_POSITIVE:
		case controllerRef::JOY_1_AXIS_Y_NEGATIVE:
		case controllerRef::JOY_2_AXIS_X_NEGATIVE:
		case controllerRef::JOY_2_AXIS_X_POSITIVE:
		case controllerRef::JOY_2_AXIS_Y_POSITIVE:
		case controllerRef::JOY_2_AXIS_Y_NEGATIVE:
			keyFound = GLFW_KEY_O;
			break;
		}

		int offset = keyFound - GLFW_KEY_A;
		return Renderer::GetSprite(ALetterTexture + offset);
	}

	Rendering::Sprite* DisplayIconManager::getTextureForKey(keyRef key)
	{
		int keyID = InputManager::GetKeybindValue(static_cast<int>(key));
		int offset = keyID - GLFW_KEY_A;

		return Renderer::GetSprite(ALetterTexture + offset);
	}
}

