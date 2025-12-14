#include "StaticGUIElement.h"

#include "../../!TestGame/TestGame.h"

namespace GAME_NAME
{
	namespace Objects
	{
		namespace GUI
		{
			
			void StaticGUIElement::Render(float zoom)
			{
				if (!m_active) { return; }
				m_sprite->Render(Vec2::Zero, m_position / zoom, m_scale / zoom, 0.0f);
			}
			
		}
	}
}