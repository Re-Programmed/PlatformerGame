#include "GUIScrollArea.h"
#include "../../Input/InputManager.h"

namespace GAME_NAME::Objects::GUI
{
	int GUIScrollArea::m_scrollCallbackID = -1;
	std::vector<GUIScrollArea*> GUIScrollArea::m_areas;

	GUIScrollArea::GUIScrollArea(Vec2 position, Vec2 scale, GLuint backgroundTexture, double minY, double maxY)
		: StaticGUIElement(position, scale, backgroundTexture), m_maxY(maxY), m_minY(minY)
	{
		m_contents.reserve(5);

		if (m_scrollCallbackID < 0)
		{
			m_scrollCallbackID = InputManager::RegisterScrollCallback(scrollCallback);
		}

		m_areas.push_back(this);
	}

	GUIScrollArea::~GUIScrollArea()
	{
		m_areas.erase(std::find(m_areas.begin(), m_areas.end(), this));

		//InputManager::UnregisterScrollCallback(m_scrollCallbackID);

		StaticGUIElement::~StaticGUIElement();
	}

	void GUIScrollArea::Render(float zoom)
	{
		if (this->m_sprite->GetSpriteId() == 0)
		{
			return;
		}

		StaticGUIElement::Render(zoom);
	}

	void GUIScrollArea::scrollCallback(GLFWwindow* window, double offsetX, double offsetY)
	{
		for (GUIScrollArea* area : m_areas)
		{
			area->onScroll(window, offsetX, offsetY);
		}
	}

	void GUIScrollArea::onScroll(GLFWwindow* window, double offsetX, double offsetY)
	{
		if (m_currentY + offsetY >= m_maxY || m_currentY + offsetY <= m_minY)
		{
			return;
		}

		for (StaticGUIElement* element : m_contents)
		{
			element->SetPosition(element->GetPosition() + Vec2{ 0.f, (float)offsetY });
		}

		m_currentY += offsetY;
	}
}

