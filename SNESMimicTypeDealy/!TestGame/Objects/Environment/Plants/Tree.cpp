#include "Tree.h"
#include "../../../../Resources/Save/SaveManager.h"
#include "../../../../Objects/StateSaver.h"
#include "../../../Items/FloorItem.h"
#include "../../../../Components/Animation/AnimatorComponent.h"
#include "../../../../Utils/Time/GameTime.h"
#include "../../../InputDisplay/DisplayIconManager.h"
#include "../../../Items/Types/Tool.h"
#include "../../../../Objects/Particles/ParticleEmitter.h"
#include "../../../../Objects/Instantiate/LevelObjectHandler.h"

#include "../../../Items/Types/Food.h"

namespace GAME_NAME::Objects::Environment::Plants
{

	Tree::Tree(Vec2 position, Vec2 scale, Rendering::Sprite* sprite, size_t saveID, Rendering::Sprite* choppedSprite)
		: Interactable(TREE_INTERACT_KEY, InputManager::KEY_STATE_NONE, position, scale, sprite), GameObjectState(saveID),
		m_isChopped(false), m_choppedSprite(choppedSprite), m_rustles(0)
	{
		LoadState();

		AnimData rustleData;
		rustleData.Sprites.emplace_back(std::make_shared<Sprite>(m_sprite->GetSpriteId() + 1));
		rustleData.Sprites.emplace_back(std::make_shared<Sprite>(m_sprite->GetSpriteId() + 2));
		std::shared_ptr<GAME_NAME::Components::Animation::Animation> rustle(new GAME_NAME::Components::Animation::Animation(rustleData, 0.2f));

		std::vector<std::shared_ptr<GAME_NAME::Components::Animation::Animation>> anims{ rustle };

		m_animator = new AnimatorComponent(anims);
	}

	void Tree::SaveState()
	{
		std::string saveStr = "";
		saveStr = saveStr + SAVE_MNG_STATE(m_isChopped);
		saveStr = saveStr + std::to_string(m_rustles);

		Resources::SaveManager::SaveLevelString(saveStr, m_objectSaveID);
	}

	void Tree::LoadState()
	{
		std::string result(SAVE_MNG_FALSE_STATE + "");
		Resources::SaveManager::GetLevelString(result, m_objectSaveID);
		setChopped(result[0] == SAVE_MNG_TRUE_STATE);

		if (result.length() > 1)
		{
			m_rustles = std::stoi(std::string("0") + result[1]);
		}
	}

	void Tree::Update(GLFWwindow* window)
	{
		Interactable::Update(window);

		m_animator->Update(window, this);
	}

	bool Tree::Chop()
	{
		if (m_isChopped) { return false; }

		setChopped(true);

		//DROP LOG OR APPLE
		Items::FloorItem* myItem = new Items::FloorItem(m_position + Vec2::RandVec2(m_scale.X, m_scale.Y), new InventoryItem(Items::LOG), 0.5f);
		Renderer::InstantiateObject(Renderer::InstantiateGameObject(myItem, true, 1, false));

		if (!m_toBeSaved)
		{
			StateSaver::RegisterToBeSaved(this);
		}

using namespace Particles;
		
		auto objs = Instantiate::LevelObjectHandler::GetLevelObjects("tree_particles");
		
		for (GameObject* obj : objs)
		{
			Particle particle1 = std::move(obj);
			//delete obj;

			particle1.Velocity = Vec2((float)(std::rand() * 100 / RAND_MAX) / 100.f - 0.5f, 1.f + (float)(std::rand() * 100 / RAND_MAX) / 100.f - 0.5f);
			particle1.RotationalVelocity = static_cast<float>((std::rand() * 12 / RAND_MAX) - 6);
			particle1.ConstantVelocity = Vec2((float)(std::rand() * 100 / RAND_MAX) / 100.f - 0.5f, -0.3f);
			particle1.TargetOpacity = 0.f;
			particle1.TargetScale = 0.f;

			ParticleEmitter* emitter = new ParticleEmitter(this->m_position, 1.f);
			emitter->SetScale({ 7.f, 6.f });
			emitter->RegisterParticle(particle1);

			Renderer::LoadActiveObject(emitter);

			emitter->SpawnParticles(12);

			Renderer::DestroyActiveObjectImmediate(obj);
			if (obj != nullptr) {
				Renderer::DestroyObject(obj);
			}
		}

		return true;
	}

	bool Tree::Rustle()
	{
		m_rustleCounter += (float)Utils::Time::GameTime::GetScaledDeltaTime();
		m_animator->SetCurrentAnimation(0);

		if (m_rustleCounter > 1.85f)
		{
			m_rustleCounter = 0;

			m_animator->SetCurrentAnimation(-1);

			if (m_rustles > 2) { return false; }
					//Increment counter until full to shake tree for period of time.

			m_rustles++;

			int c = 1 + (std::rand() * 3 / RAND_MAX);
			for (int i = 0; i < c; i++)
			{
				if (std::rand() < RAND_MAX / 2)
				{
					Items::FloorItem* myItem = new Items::FloorItem(m_position + Vec2::RandVec2(m_scale.X, m_scale.Y), new Items::InventoryItem(Items::LEAVES), 0.5f);
					Renderer::InstantiateObject(Renderer::InstantiateGameObject(myItem, true, 1, false));
				}
				else {
					Items::FloorItem* myItem = new Items::FloorItem(m_position + Vec2::RandVec2(m_scale.X, m_scale.Y), new Items::Food(Items::APPLE, 20), 0.5f);
					Renderer::InstantiateObject(Renderer::InstantiateGameObject(myItem, true, 1, false));
				}
			}

			return true;
		}
		
		return false;
		

	}

	void Tree::onInteract(std::shared_ptr<Player::Player> player, InputManager::KEY_STATE state)
	{
		if (m_isChopped) { return; }

		if (InputManager::GetMouseButton(0))
		{
			auto heldItem = TestGame::ThePlayer->GetInventory()->GetHeldItem();
			if (heldItem != nullptr && Items::ITEMTYPE_GetItemData(heldItem->GetType()).Actions & Items::TOOL_ACTION::CHOP)
			{
				double sdt = Utils::Time::GameTime::GetScaledDeltaTime();
				m_chopTimer += sdt;

				//Make tree shake back and forth.
				m_choppingAnimationTimer += sdt;
				if (m_choppingAnimationTimer >= 0.3)
				{
					SetPosition(this->GetPosition() + Vec2{ 1.f, 0 });
					m_choppingAnimationTimer = 0.0;
				}
				else if (m_choppingAnimationTimer > 0.05 && m_choppingAnimationTimer < 0.075)
				{
					SetPosition(this->GetPosition() + Vec2{ -2.f, 0 });
					m_choppingAnimationTimer = 0.075;
				}
				else if (m_choppingAnimationTimer > 0.2 && m_choppingAnimationTimer < 0.25)
				{
					SetPosition(this->GetPosition() + Vec2{ 1.f, 0 });
					m_choppingAnimationTimer = 0.25;
				}



				if (m_chopTimer > 5.0)
				{
					Chop();
				}
			}

			return;
		}

		if (m_rustles > 2) { return; }

		Input::DisplayIconManager::ShowKeyInputDisplay(keyRef::PLAYER_INTERACT, TestGame::ThePlayer->GetPosition() + Vec2(TestGame::ThePlayer->GetScale() + Vec2(3, -5)), (char)(std::floor(m_rustleCounter * 10 / 1.85f)));

		if (state & InputManager::KEY_STATE_HELD)
		{
			bool suc = Rustle();
			if (suc && !m_toBeSaved)
			{
				StateSaver::RegisterToBeSaved(this);
			}
		}
		else if (state & InputManager::KEY_STATE_RELEASED)
		{
			m_animator->SetCurrentAnimation(-1);
			m_rustleCounter = 0;
		}


	}

	void Tree::setChopped(bool chopped)
	{
		if (m_isChopped == chopped) { return; }

		m_isChopped = chopped;

		//FLIP m_choppedSprite and m_sprite
		const std::shared_ptr<Rendering::Sprite> spCpy = std::shared_ptr<Rendering::Sprite>(new Rendering::Sprite(m_choppedSprite->GetSpriteId()));

		this->m_choppedSprite = m_sprite;

		m_sprite = spCpy;
	}
}


