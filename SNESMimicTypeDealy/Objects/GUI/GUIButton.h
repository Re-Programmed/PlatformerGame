#pragma once
#ifndef _GUI_BUTTON
#define _GUI_BUTTON
#include "../../Utils/Math/Vec2.h"
#include "../../Rendering/Sprite.h"
#include "StaticGUIElement.h"
#include <functional>

namespace GAME_NAME
{
	using namespace MathUtils;
	namespace Objects
	{
		namespace GUI
		{
			/// <summary>
			/// A GUI element that will not update.
			/// </summary>
			class GUIButton : public StaticGUIElement

			{
			public:
				GUIButton(Vec2 position, Vec2 scale, Sprite* sprite, std::function<void(int)>* onClick, Vec4 baseColor = { 1.f, 1.f, 1.f, 1.f }, Vec4 hoverColor = { 0.5f, 0.5f, 0.f, 1.f })
					:StaticGUIElement(position, scale, sprite), OnClick(onClick), m_buttonId(-1), m_baseColor(baseColor), m_hoverColor(hoverColor), OnRightClick(nullptr)
				{

				}

				GUIButton(Vec2 position, Vec2 scale, GLuint texture, std::function<void(int)>* onClick, Vec4 baseColor = { 1.f, 1.f, 1.f, 1.f }, Vec4 hoverColor = { 0.5f, 0.5f, 0.f, 1.f })
					:StaticGUIElement(position, scale, texture), OnClick(onClick), m_buttonId(-1), m_baseColor(baseColor), m_hoverColor(hoverColor), OnRightClick(nullptr)
				{

				}

				GUIButton(Vec2 position, Vec2 scale, GLuint texture, std::function<void(int)>* onClick, int buttonId, Vec4 baseColor = { 1.f, 1.f, 1.f, 1.f }, Vec4 hoverColor = { 0.5f, 0.5f, 0.f, 1.f })
					:StaticGUIElement(position, scale, texture), OnClick(onClick), m_buttonId(buttonId), m_baseColor(baseColor), m_hoverColor(hoverColor), OnRightClick(nullptr)
				{

				}

				std::function<void(int buttonId)> *OnClick, *OnRightClick;

				inline void SetOnRightClick(std::function<void(int buttonId)>* onRightClick)
				{
					OnRightClick = onRightClick;
				}
				
				inline void SetButtonId(unsigned int id)
				{
					m_buttonId = id;
				}

				inline int GetButtonId()
				{
					return m_buttonId;
				}

				inline void SetHoverColor(Vec4 color)
				{
					m_hoverColor = color;
				}

				inline const Vec4& GetHoverColor()
				{
					return m_hoverColor;
				}

				inline void SetBaseColor(Vec4 color)
				{
					m_baseColor = color;
				}

				inline const Vec4& GetBaseColor()
				{
					return m_baseColor;
				}

				void Render(float zoom) override;	//Overriden to have detection for hovering.

			private:
				/// <summary>
				/// The button id, used to dertermine which button is getting interacted with when a GUI callback is passed.
				/// </summary>
				unsigned int m_buttonId;

				/// <summary>
				/// Represents the color that the button is tinted when it is hovered.
				/// </summary>
				Vec4 m_hoverColor, m_baseColor;

				/// <summary>
				/// Lerp Timer is used to store how much between states the button is.
				/// This creates smooth animations between states for buttons.
				/// </summary>
				float m_lerpTimer = 0.f;
			};
		}
	}
}
#endif