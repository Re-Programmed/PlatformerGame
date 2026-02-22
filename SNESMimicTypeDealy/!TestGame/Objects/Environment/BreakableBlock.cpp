#include "BreakableBlock.h"

#include "../../../Input/InputManager.h"
#include "../../TestGame.h"
#include "../../../Utils/CollisionDetection.h"

#include "../../../Utils/Time/GameTime.h"

#include "../../../Resources/Save/SaveManager.h"
#include "../../../Objects/StateSaver.h"

#include "../../Items/FloorItem.h"

#define BREAKABLE_BLOCK_FADE_SPEED_MULTIPLIER 5.f

#define BREAKABLE_BLOCK_EXPLOSION_RESISTANCE 5

namespace GAME_NAME::Objects
{
	double BreakableBlock::m_currentMiningTime = 0.0;

	BreakableBlock::BreakableBlock(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, size_t saveID, double mineTime, int mineResistance, TOOL_ACTION requiredActionFlag, InventoryItem* drops)
		: StaticBoxCollisionObject(position, scale, sprite), GameObjectState(saveID), m_mineTime(mineTime), m_mineResistance(mineResistance), m_requiredActionFlag(requiredActionFlag), m_isBroken(false), m_shakeOffset(Vec2{0.f}),
		m_drops(drops)
	{
		this->LoadState();
	}

	void BreakableBlock::Render(const Vec2& cameraPosition)
	{
		//If its broken, no need to do anything.
		if (m_isBroken) { return; }

		//Render outline if hovered.
		if(m_isHovered || m_hoverDisplayPercent > 0.0)
		{
			if (m_isHovered)
			{
				m_hoverDisplayPercent += Utils::Time::GameTime::GetScaledDeltaTime() * BREAKABLE_BLOCK_FADE_SPEED_MULTIPLIER;
				if (m_hoverDisplayPercent > 1.0) { m_hoverDisplayPercent = 1.0; }
			}
			else {
				m_hoverDisplayPercent -= Utils::Time::GameTime::GetScaledDeltaTime() * BREAKABLE_BLOCK_FADE_SPEED_MULTIPLIER;
			}

			DynamicSprite* hoveredSprite = new DynamicSprite(Renderer::GetSprite(72)->GetSpriteId());

			Vec4 hoverColor = Vec4::LerpBetween({ 1.f, 1.f, 0.f, 0.f }, { 1.f, 1.f, 0.f, 1.f }, m_hoverDisplayPercent);
			Vec4 hoverVerts[4] = {
				hoverColor,hoverColor,hoverColor,hoverColor
			};

			hoveredSprite->UpdateTextureColor(hoverVerts);

			hoveredSprite->Render(cameraPosition, m_position - Vec2(1), m_scale + Vec2(2));
		}
		
		this->SetPosition(this->GetPosition() + m_shakeOffset);
		GameObject::Render(cameraPosition);
		this->SetPosition(this->GetPosition() - m_shakeOffset);
	}

	void BreakableBlock::Update(GLFWwindow* window)
	{
		//If its broken, no need to do anything.
		if (m_isBroken) { return; }

		if (InputManager::GetMouseHidden())
		{
			return;
		}

		Vec2 mouseWorldPosition = InputManager::GetMouseWorldPosition(TestGame::INSTANCE->GetCamera());
		m_isHovered = Utils::CollisionDetection::PointWithinBoxBL(mouseWorldPosition, m_position, m_scale);

		if (InputManager::GetKey(PLAYER_USE_ITEM))
		{
			//The player is trying to mine this object.
			if (m_isHovered)
			{
				if (TestGame::ThePlayer->GetInventory()->GetHeldItem() == nullptr) { m_currentMiningTime = 0; return; }
				const ItemData data = ITEMTYPE_GetItemData(TestGame::ThePlayer->GetInventory()->GetHeldItem()->GetType());

				//Can the current held item mine stuff?
				if (data.Actions & m_requiredActionFlag)
				{
					//Ensure the item has a value for the given data registered.
					if (data.Attributes.contains(m_requiredActionFlag))
					{

						//Does the current held item have enough mining power?
						if (std::stoi(data.Attributes.at(m_requiredActionFlag)) >= m_mineResistance)
						{
							if (m_shakeTimer > 0.05)
							{
								if (m_shakeOffset.X > 0)
								{
									m_shakeOffset.X = -0.5f;
								}
								else {
									m_shakeOffset.X = 0.5f;
								}

								m_shakeTimer = 0.0;
							}

							float sdt = Utils::Time::GameTime::GetScaledDeltaTime();
							m_currentMiningTime += sdt;
							m_shakeTimer += sdt;
						}
					}
				}

				//TODO: Create a meter or graphic of some kind.

				//Break this block.
				if (m_currentMiningTime >= m_mineTime)
				{

					breakBlock();
					
					/*
						Was broken, we need to make sure that is saved.
					*/
					if (!m_toBeSaved)
					{
						StateSaver::RegisterToBeSaved(this);
					}

					//Reset mining time.
					m_currentMiningTime = 0;
				}
			}
		}
		else {
			//Reset mining time.
			m_currentMiningTime = 0;
			m_shakeOffset = 0;
		}

		StaticBoxCollisionObject::Update(window);
	}

	void BreakableBlock::Exploded(Vec2 origin, float power)
	{
		if (m_mineResistance < BREAKABLE_BLOCK_EXPLOSION_RESISTANCE)
		{
			breakBlock();
		}
	}

	void BreakableBlock::SaveState()
	{
		std::string saveStr("");
		saveStr = saveStr + SAVE_MNG_STATE(m_isBroken);
		Resources::SaveManager::SaveLevelString(saveStr, m_objectSaveID);
	}

	void BreakableBlock::LoadState()
	{
		std::string result(SAVE_MNG_FALSE_STATE + ""); //Default is not broken.
		Resources::SaveManager::GetLevelString(result, m_objectSaveID); //Load save.

		if (result[0] == SAVE_MNG_TRUE_STATE)
		{
			breakBlock(true);
		}
	}

	void BreakableBlock::breakBlock(bool loadingFromSave)
	{
		if (m_isBroken) { return; }

		if (!loadingFromSave)
		{
			//Create particles.
			Particles::ParticleEmitter* pe = new Particles::ParticleEmitter(m_position, 5.f, true);
			Renderer::LoadActiveObject(pe);

			Particles::Particle particle = Particles::Particle(Vec2::Zero, m_scale / 1.5f, 0.f, { 0.f, 0.f }, 0.f, 0.75f, new Sprite(this->m_sprite->GetSpriteId()));
			particle.TargetOpacity = 0.f;

			pe->RegisterParticle(particle);
			pe->SpawnParticles(std::rand() * 10 / RAND_MAX + 10, { 0.45f, 1.f }, 0.0225f, 0.f);

			if (m_drops != nullptr)
			{
				FloorItem* item = new FloorItem(m_position + m_scale / 2.f, m_drops, 1.f);
				Renderer::InstantiateObject(Renderer::InstantiateGameObject(item, true, 1, false));
			}

			//BreakableBlock* removeAfterTime = this;
			//Create thread that sleeps until it can delete the particle emitter.
			std::thread particleEmitterRemoval([pe/*, removeAfterTime */]() {
				std::this_thread::sleep_for(std::chrono::seconds(8));
				Renderer::DestroyActiveObjectImmediate(pe);
				//delete pe;
				//delete removeAfterTime;
				});

			particleEmitterRemoval.detach();
		}

		//Set broken.
		m_isBroken = true;

		/*
			Necessary because DestroyObject also deletes the object (for some reason)
			The object must still save before the game closes so it must exist even if destroyed unless it dosent need updated in the case that it was already broken.
		*/
		if (loadingFromSave)
		{
			//Destroy this object.
			Renderer::DestroyObject(this);
		}

	}
}
