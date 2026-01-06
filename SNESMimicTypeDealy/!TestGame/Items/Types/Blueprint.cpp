#include "Blueprint.h"

#include "../Crafting/CraftingDataManager.h"

namespace GAME_NAME::Items
{
    Blueprint::Blueprint(ITEM_TYPE type, int recipeID)
        : InventoryItem(type), m_recipeID(recipeID)
    {

    }
    MiscState::SaveParam Blueprint::Encode()
    {
        //b(type)+(recipeID)

        return InventoryItem::Encode().append(std::string("+").append(std::to_string(m_recipeID)));
    }

    void Blueprint::Decode(const SaveParam params)
    {
        m_recipeID = std::stoi(params.substr(params.find_first_of('+') + 1));
        InventoryItem::Decode(ITEM_PREFIX_ITEM + params.substr(1, params.find('+') - 1));
    }

    bool Blueprint::Use()
    {
        Crafting::Recipe& recipe = Crafting::CraftingDataManager::GetRecipe(m_recipeID);

        recipe.Unlocked = true;

        return true;
    }

    std::unique_ptr<InventoryItem> Blueprint::GetUnique()
    {
        return std::unique_ptr<InventoryItem>(this);
    }

}
