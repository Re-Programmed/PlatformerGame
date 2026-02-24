#include "SoundMaterial.h"

#include "../TestGame.h"

namespace GAME_NAME::Objects
{
    std::unordered_map<std::string, std::unordered_map<SoundMaterial::Material, std::vector<GLuint>>> SoundMaterial_MaterialDatabase = {
        { "/green_region", { 
            { SoundMaterial::Material::Grass, { 6, 7, 8, 9, 102, 104, 105, 106, 107 } },
            { SoundMaterial::Material::Wood, { 85, 115, 27, 28, 29, 34, 35, 36, 39, 40, 45, 46, 47, 11 } }
        } },
        { "/green_region_area_2", {
            { SoundMaterial::Material::Grass, { 6, 7, 8, 9, 102, 104, 105, 106, 107 } },
            { SoundMaterial::Material::Wood, { 85, 115, 27, 28, 29, 34, 35, 36, 39, 40, 45, 46, 47, 11 } }
        } }
    };

    

    SoundMaterial::Material SoundMaterial::GetSoundMaterial(GameObject* object)
    {
        std::string levelPath = TestGame::INSTANCE->GetCurrentLevelPath();
        if (SoundMaterial_MaterialDatabase.contains(levelPath))
        {
            for (auto& [material, textures] : SoundMaterial_MaterialDatabase[levelPath])
            {
                for (int textureId : textures)
                {
                    if (Renderer::GetSpriteIdFromTextureId(textureId) == object->GetSprite()->GetSpriteId())
                    {
                        return material;
                    }
                }
            }
        }

        return SoundMaterial::Material::None;
    }

    Audio::SoundEvents::Event SoundMaterial::GetWalkEffectForMaterial(Material material)
    {
        switch (material)
        {
        case SoundMaterial::Material::Grass:
            return Audio::SoundEvents::Event::OBJECT_WALK_GRASS;
        case SoundMaterial::Material::Wood:
        default:
            return Audio::SoundEvents::Event::OBJECT_WALK_WOOD;
        }
    }

    Audio::SoundEvents::Event SoundMaterial::GetHitEffectForMaterial(Material material)
    {
        switch (material)
        {
        case SoundMaterial::Material::Grass:
            return Audio::SoundEvents::Event::OBJECT_LAND_GRASS;
        case SoundMaterial::Material::Wood:
        default:
            return Audio::SoundEvents::Event::OBJECT_LAND_WOOD;
        }
    }
}
