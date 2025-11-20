#pragma once

#include "../Objects/GUI/Menus/GUIMenu.h"
#include "../Rendering/Renderers/Renderer.h"
#include "../Objects/GUI/Text/TextRenderer.h"

#include "../Objects/GUI/StaticGUIElement.h"

namespace GAME_NAME
{
	/// <summary>
	/// A bunch of methods to manage the navigation and loading of settings screens.
	/// </summary>
	class SettingsManager
	{
	public:
		/// <summary>
		/// Closes the settings menu.
		/// </summary>
		inline static void CloseMenu(bool ignoreRemoval = false)
		{
			m_settingsMenuOpen = false;

			if (!ignoreRemoval) { GUI::Menus::GUIMenu::RemoveLastMenu(); }

			//Renable all objects to RESUME GAME and PHYSICS.
			Rendering::Renderer::UpdateObjects = true;
		}

		/// <summary>
		/// Opens the main settings menu.
		/// </summary>
		static void CreateSettingsMenu();

		static void OpenSoundMenu();
		
		static void OpenGraphicsMenu();

		static void Update();

		/// <summary>
		/// Opens the controls menu for editing.
		/// </summary>
		static void OpenControlsMenu();

		/// <summary>
		/// True if a page of the settings menu is open.
		/// </summary>
		/// <returns></returns>
		inline static bool SettingsMenuIsOpen() { return m_settingsMenuOpen; }

	private:
		/// <summary>
		/// A list of currently loaded elements on the current page.
		/// Useful for adding scrolling or removing elements when changing menus.
		/// </summary>
		static std::vector<GUI::StaticGUIElement*> m_currentPageElements;

		static std::vector<GUI::StaticGUIElement*> m_controlsPageNumbers;

		/// <summary>
		/// The text in the bottom right that shows that the game is awaiting a key.
		/// </summary>
		static std::vector<GUI::StaticGUIElement*> m_recordingText;

		static std::vector<GUI::StaticGUIElement*> m_musicMixerPercentage;
		static std::vector<GUI::StaticGUIElement*> m_sfxMixerPercentage;

		/// <summary>
		/// True if the settings menu is currently open.
		/// </summary>
		static bool m_settingsMenuOpen;

		/// <summary>
		/// Callback for main settings page.
		/// </summary>
		/// <param name="id"></param>
		static void mainMenu_guiCallback(int id);
		/// <summary>
		/// Callback for the controls list page.
		/// </summary>
		/// <param name="id"></param>
		static void controlsMenu_guiCallback(int id);

		static void soundMenu_guiCallback(int id);

		static void graphicsMenu_guiCallback(int id);

		static GUI::Text::TextRenderer::RenderedDigit UpdateAudioMixer(int mixerIndex, int& percentDisplay, GUI::Text::TextRenderer::RenderedDigit& const initialDigit);

		/// <summary>
		/// The GLFW scroll callback for the controls menu. Used for scrolling through pages of controls.
		/// </summary>
		static void controlsMenu_scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

		/// <summary>
		/// True if the settings menu is expecting a key input to update a key.
		/// </summary>
		static bool m_recordingKey;
		
		/// <summary>
		/// The current setting that is expecting an input.
		/// </summary>
		static int m_currentRecordingIndex;

	};
}