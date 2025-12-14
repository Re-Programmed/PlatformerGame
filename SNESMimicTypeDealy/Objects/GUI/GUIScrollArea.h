#pragma once

#include "./StaticGUIElement.h"

namespace GAME_NAME::Objects::GUI
{
	class GUIScrollArea
		: public StaticGUIElement
	{
	public:

		GUIScrollArea(Vec2 position, Vec2 scale, GLuint backgroundTexture = 0, double minY = 0, double maxY = 0);

		~GUIScrollArea();

		void Render(float zoom) override;

		inline void AddElement(StaticGUIElement* element)
		{
			m_contents.push_back(element);
		}

		inline void RemoveElement(StaticGUIElement* element)
		{
			m_contents.erase(std::find(m_contents.begin(), m_contents.end(), element));
		}

	protected:
		const double m_minY, m_maxY;
		double m_currentY = 0;

		void onScroll(GLFWwindow* window, double offsetX, double offsetY);

		static void scrollCallback(GLFWwindow* window, double offsetX, double offsetY);
		static int m_scrollCallbackID;

		std::vector<StaticGUIElement*> m_contents;

		static std::vector<GUIScrollArea*> m_areas;
	};
}