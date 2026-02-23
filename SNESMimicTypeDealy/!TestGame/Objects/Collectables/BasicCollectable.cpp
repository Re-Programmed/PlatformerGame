#include "BasicCollectable.h"

#include "../../../Input/InputManager.h"
#include "../../../Resources/Save/SaveManager.h"

#include "../../../Utils/Time/GameTime.h"

#include "../../../Objects/StateSaver.h"

#define BASIC_COLLECTABLE_GLOW_SPRITE SpriteBase(131)

namespace GAME_NAME::Objects::Collectables
{
	BasicCollectable::BasicCollectable(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, size_t saveId)
		: Interactable(PLAYER_INTERACT, InputManager::KEY_STATE_NONE, scale.X + 8.f, position, scale, sprite), GameObjectState(saveId)
	{
		LoadState();
	}

	void BasicCollectable::LoadState()
	{
		//Load if this collectable has already been gotten.
		std::string result(SAVE_MNG_FALSE_STATE + "");
		Resources::SaveManager::GetLevelString(result, m_objectSaveID);

		m_isCollected = result.starts_with(SAVE_MNG_TRUE_STATE);
		if (m_isCollected) 
		{
			m_wasPreviouslyCollected = true;
		}
	}

	void BasicCollectable::SaveState()
	{
		std::string saveStr = "";
		saveStr = saveStr + SAVE_MNG_STATE(m_isCollected);

		Resources::SaveManager::SaveLevelString(saveStr, m_objectSaveID);
	}

#define BASIC_COLLECTABLE_FLY_LERP_SPEED 4.6

	void BasicCollectable::Update(GLFWwindow* window)
	{
		if (m_wasPreviouslyCollected) { return; }

		if (m_isCollected)
		{
			Vec2 center = m_position + m_scale / 2.f;
			Vec2 playerCenter = TestGame::ThePlayer->GetPosition() + TestGame::ThePlayer->GetScale() / 2.f;

			center = Vec2::Lerp(center, playerCenter, Utils::Time::GameTime::GetScaledDeltaTime() * BASIC_COLLECTABLE_FLY_LERP_SPEED);

			this->SetPosition(center - m_scale / 2.f);

			if (Vec2::Distance(center, playerCenter) < 6.5f)
			{
				m_active = false;
			}
		}

		Interactable::Update(window);
	}

	Vec4 BasicCollectable_PreviouslyCollectedColors[4] = {
		{ 0.6f, 0.6f, 1.f, 0.5f },
		{ 0.6f, 0.6f, 1.f, 0.5f },
		{ 0.6f, 0.6f, 1.f, 0.5f },
		{ 0.6f, 0.6f, 1.f, 0.5f }
	};
	
	void BasicCollectable::Render(const Vec2& cameraPosition)
	{
		if (m_wasPreviouslyCollected)
		{	
			DynamicSprite sprite(m_sprite->GetSpriteId());
			sprite.UpdateTextureColor(BasicCollectable_PreviouslyCollectedColors);
			sprite.Render(cameraPosition, m_position + m_scale, m_scale * Vec2{ -1.f });
			return;
		}


		Sprite* glow = Renderer::GetSprite(BASIC_COLLECTABLE_GLOW_SPRITE);
		Vec2 glowScale = m_scale * 1.25f;
		glow->Render(cameraPosition, (m_position + m_scale/2.f) - (glowScale / 2.f), glowScale);
		delete glow;

		Interactable::Render(cameraPosition);
	}

	void BasicCollectable::onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state)
	{
		if (m_isCollected) { return; }

		//The collectable was collected...

		m_isCollected = true;		   //Set collected.
		m_scale = m_scale / 2.4f;
		StateSaver::RegisterToBeSaved(this); //Prep to save the object.

		onCollect();				   //Callback.
	}

}