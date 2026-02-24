#pragma once
#include "../../Objects/GameObject.h"
#include "../../Rendering/Renderers/Renderer.h"

#include "../../Input/InputManager.h"
#include "../../Objects/GUI/Text/TextRenderer.h"

#include <unordered_map>

namespace GAME_NAME::Input
{
	class DisplayIconManager
	{
	public:
		/// <summary>
		/// Render a key prompt with progress.
		/// </summary>
		/// <param name="keyDisplay"></param>
		/// <param name="anchor"></param>
		static void ShowKeyInputDisplay(keyRef key, Vec2 anchor, char progress = 0, std::string optionDescription = "");
		
		/// <summary>
		/// Called each frame to destroy any icons that are no longer in use.
		/// </summary>
		static void AttemptHideIcons();
	private:
		struct KeyDisplay
		{
			unsigned int Index;

			Objects::GameObject* BGDisplayObject;	//The game object that is copied to represent each key icon.
			Objects::GameObject* TextDisplayObject;

			GUI::Text::TextRenderer::ExpectedRenderedWord OptionEventDescription;
			GUI::StaticGUIElement* OptionEventDescriptionBG;

			bool WasDrawnOnLastFrame = true;
		};

		static std::unordered_map<keyRef, KeyDisplay> m_keyDisplays;

		static const int m_keyDisplayBaseTexture;	//The texture id with 0 progress.		

		static Rendering::Sprite* getTextureForControllerButton(controllerRef controllerButton);
		static Rendering::Sprite* getTextureForKey(keyRef key);
	};
}