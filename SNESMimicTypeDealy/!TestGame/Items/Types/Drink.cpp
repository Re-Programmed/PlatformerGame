#include "Drink.h"

namespace GAME_NAME::Items
{
	DrinkEffect Drink::GetEffect()
	{
		DrinkEffectType type(DrinkEffectType::STRENGTH);
		int amplifier(0);
		double duration(0.0);
		
		std::string data = ITEMTYPE_GetItemData(this->m_itemType).Attributes.at(TOOL_ACTION::DRINK);
		std::stringstream decode(data);

		std::string dataPoint;
		int i = 0;
		while (std::getline(decode, dataPoint, ','))
		{
			switch (i++)
			{
			case 0:
				type = static_cast<DrinkEffectType>(std::stoi(dataPoint));
				break;
			case 1:
				amplifier = std::stoi(dataPoint);
				break;
			case 2:
				duration = std::stod(dataPoint);
				break;
			default:
				break;
			}
		}

		Sprite* sp = ITEMTYPE_GetItemTypeTexture(m_itemType);
		GLuint textureID = sp->GetSpriteId();
		delete sp;

		return DrinkEffect{ type, amplifier, duration, textureID };
	}

}
