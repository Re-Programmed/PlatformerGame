#include "Blueprint.h"

#include "../Crafting/CraftingDataManager.h"

#include "../../Cutscenes/DialogueManager.h"

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

        std::string itemName = ITEMTYPE_GetItemTypeName(recipe.Return->GetType());
        Cutscenes::DialogueManager::INSTANCE->PlayDialogueSequence(Cutscenes::DialogueSequence(1, Cutscenes::DialogueSequence::DialogueEvent("I can make a " + itemName + " %item_" + std::to_string(recipe.Return->GetType()) + " now!", TestGame::ThePlayer.get())));

        return true;
    }

    std::unique_ptr<InventoryItem> Blueprint::GetUnique()
    {
        return std::unique_ptr<InventoryItem>(this);
    }

}
