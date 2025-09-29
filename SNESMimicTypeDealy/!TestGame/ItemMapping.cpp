#include "ItemMapping.h"

#include <sstream>
#include "./Items/Types/Tool.h"
#include "./Items/Types/Weapon.h"
#include "./Items/Types/Food.h"
#include "./Items/Types/Placeable.h"

namespace GAME_NAME
{
	template<typename Base, typename T>
	inline bool instanceof(const T* ptr) {
		return dynamic_cast<const Base*>(ptr) != nullptr;
	}

	InventoryItem* ItemMapping::DeSerialize(std::string itemString)
	{
using namespace std;

		//Get the first char apart from the parenthesies to determine the type of item.
		const char firstChar = itemString.at(1);

		//Remove the first segment, should be '(t_'
		itemString.erase(0, 3);

		//Remove ending ")".
		if (itemString.ends_with(")")) { itemString.erase(itemString.size() - 1, itemString.size()); }

		//Create a list of all item parameters.
		stringstream ss(itemString);
		string param;

		vector<string> parameters;

		while(getline(ss, param, ';'))
		{
			parameters.push_back(param);
		}


		//Check the first character to determine what type of item this is. 
		switch (firstChar)
		{
		case ITEM_PREFIX_TOOL:	//Tool
			return new Tool((ITEM_TYPE)std::stoi(parameters[0]), std::stoi(parameters[1]));

		case ITEM_PREFIX_WEAPON: //Weapon
			return new Weapon((ITEM_TYPE)std::stoi(parameters[0]), std::stoi(parameters[1]));

		case ITEM_PREFIX_FOOD:
			return new Food((ITEM_TYPE)std::stoi(parameters[0]), std::stoi(parameters[1]));

		case ITEM_PREFIX_PLACEABLE:
			return new Placeable((ITEM_TYPE)std::stoi(parameters[0]));
			
		case ITEM_PREFIX_ITEM:
		default:
			return new InventoryItem((ITEM_TYPE)std::stoi(parameters[0]));
		}
	}
}