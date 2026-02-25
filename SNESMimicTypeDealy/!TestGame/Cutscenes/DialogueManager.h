#pragma once

#include "DialogueSequence.h"
#include "../../Objects/GUI/StaticGUIElement.h"
#include "../../Objects/GUI/Text/TextRenderer.h"
#include "../TestGame.h"
#include "../../Objects/GUI/Menus/GUIMenu.h"

#include "../../Utils/ConstUpdateable.h"

using namespace GAME_NAME::Objects::GUI;

#define DIALOGUE_TEXT_BOX_SPACING_X 40				//How far the dialogue box is from the edges (X).
#define DIALOGUE_TEXT_BOX_SPACING_Y 5				//How far the dialogue box is from the edges (Y).
#define DIALOGUE_TEXT_BOX_TEXTURE SpriteBase(-1)	//The default texture for the dialogue text box.

namespace GAME_NAME::Cutscenes
{
	/// <summary>
	/// The DialogueManager has the ability to display dialogue events and sequences in order that the player can click through.
	/// </summary>
	class DialogueManager
		: public Utils::ConstUpdateable
	{
	public:
		//Singleton.
		static DialogueManager* INSTANCE;

		//Create the singleton.
		static void Init()
		{
			if (INSTANCE != nullptr) { return; }
			DialogueManager* d = new DialogueManager();
			INSTANCE = d;
		}

		static std::vector<Text::TextRenderer::ExpectedLetter*> RenderCaseSensitiveTextWithItemTextures(std::string word, Vec2 position, const float scale, const float& letterPadding, int layer, int uppercaseFont, int lowercaseFont, std::chrono::milliseconds letterAppearanceSpeed);

		// Begins a dialogue sequence and displays the first event.
		inline bool PlayDialogueSequence(DialogueSequence sequence)
		{
			if (m_playingDialogueSequence)
			{
				return false;
			}

			if (!GUI::Menus::GUIMenu::MenuIsOpen())
			{
				GUI::Menus::GUIMenu::OpenMenu();
			}

			m_currentDialogueSequence = sequence;
			m_playingDialogueSequence = true;

			//Freeze the player from doing anything during dialogue.
			TestGame::ThePlayer->SetFrozen(true);

			createDialogueBox();
			return true;
		}

		DialogueManager()
			: m_guiDisplay(nullptr, NULL), m_currentDialogueSequence(0, DialogueSequence::DialogueEvent("", nullptr))
		{

		}

		~DialogueManager()
		{
			if (std::get<0>(m_guiDisplay))
			{
				Renderer::UnloadGUIElement(std::get<0>(m_guiDisplay));
				delete std::get<0>(m_guiDisplay);
			}
		}

		/// <summary>
		/// Called every frame to detect when the player attempts to advance the dialogue.
		/// </summary>
		void Update(GLFWwindow* window) override;

		/// <summary>
		/// Returns a dialouge sequence that is stored in a text file from its given key.
		/// </summary>
		/// <param name="sequenceName">What dialogue sequence to get.</param>
		/// <returns></returns>
		DialogueSequence GetDialogueSequence(std::string sequenceName);

		/// <summary>
		/// Loads the dialogue sequences that are stored in the given level files.
		/// </summary>
		void LoadStoredDialogueSequences(std::string path);

		inline bool IsSequencePlaying() { return m_playingDialogueSequence; }

	private:
		//Dialogue loaded from the current level that is stored. Dialogue text is stored by separating each event with an ampersand "&."
		std::unordered_map<std::string, std::string> m_storedDialogueSequences;

		/// <summary>
		/// True if a dialogue sequence is playing.
		/// </summary>
		bool m_playingDialogueSequence = false;
		/// <summary>
		/// The current playing sequence.
		/// </summary>
		DialogueSequence m_currentDialogueSequence;

		/// <summary>
		/// The current playing event.
		/// </summary>
		static DialogueSequence::DialogueEvent m_currentDialogueEvent;

		/// <summary>
		/// True if the dialogue attempted to advance but was not done displaying. This plays a jiggle animation :)
		/// </summary>
		static bool m_jigglingText;

		/// <summary>
		/// A tuple containing onscreen elements rendered during a GUI event.
		/// </summary>
		std::tuple<StaticGUIElement*, Text::TextRenderer::ExpectedRenderedWord> m_guiDisplay;
		/// <summary>
		/// An array of all buttons on screen during a GUI selection event.
		/// </summary>
		static std::array<GUIButton*, 3> m_guiOptionButtons;

		/// <summary>
		/// Moves the dialogue to the next event.
		/// </summary>
		bool advanceDialogue();

		/// <summary>
		/// Creates buttons for a dialogue menu based on how the text for the event is separated by '/'.
		/// </summary>
		void createDialogueOptions();

		/// <summary>
		/// Stops the dialogue and removes all graphics from the screen pertaining to dialogue (calling destroyDialogueBox()).
		/// </summary>
		inline void stopDialogue()
		{
			if (!m_playingDialogueSequence)
			{
				return;
			}

			//Ensure the camera goes back to normal after the cutscene.
			static_cast<GAME_NAME::Camera::GameCamera*>(TestGame::INSTANCE->GetCamera())->UnlockCamera();
			static_cast<GAME_NAME::Camera::GameCamera*>(TestGame::INSTANCE->GetCamera())->SetTargetZoom(1.f);

			destroyDialogueBox();

			TestGame::ThePlayer->SetFrozen(false);

			m_playingDialogueSequence = false;
			m_currentDialogueSequence = DialogueSequence(0, DialogueSequence::DialogueEvent("", nullptr));
		}

		/// <summary>
		/// Creates the dialogue graphics.
		/// </summary>
		void createDialogueBox(int textBoxTexture = DIALOGUE_TEXT_BOX_TEXTURE);

		/// <summary>
		/// Removes the dialogue graphics.
		/// </summary>
		void destroyDialogueBox();

		/// <summary>
		/// Called when a dialogue button is selected during a selection event.
		/// </summary>
		static void dialogueButtonCallback(int buttonId);
	};
}